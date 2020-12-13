#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "ch8_cpu.h"
#include "ch8_opcodes.h"
#include "ch8_log.h"
#include "ch8_util.h"

// clang-format off
static const u8 font[] = {
    // 4x5 font sprites (0-F)
    0xF0, 0x90, 0x90, 0x90, 0xF0,
    0x20, 0x60, 0x20, 0x20, 0x70,
    0xF0, 0x10, 0xF0, 0x80, 0xF0,
    0xF0, 0x10, 0xF0, 0x10, 0xF0,
    0xA0, 0xA0, 0xF0, 0x20, 0x20,
    0xF0, 0x80, 0xF0, 0x10, 0xF0,
    0xF0, 0x80, 0xF0, 0x90, 0xF0,
    0xF0, 0x10, 0x20, 0x40, 0x40,
    0xF0, 0x90, 0xF0, 0x90, 0xF0,
    0xF0, 0x90, 0xF0, 0x10, 0xF0,
    0xF0, 0x90, 0xF0, 0x90, 0x90,
    0xE0, 0x90, 0xE0, 0x90, 0xE0,
    0xF0, 0x80, 0x80, 0x80, 0xF0,
    0xE0, 0x90, 0x90, 0x90, 0xE0,
    0xF0, 0x80, 0xF0, 0x80, 0xF0,
    0xF0, 0x80, 0xF0, 0x80, 0x80,
};
#define CH8_FONT_SIZE 80
// clang-format on

void ch8_reset(ch8_cpu *cpu)
{
    assert(cpu != NULL);

    // Initialize all memory to 0
    memset(cpu->memory, 0, CH8_MEM_SIZE);

    // Font data sits at beginning
    memcpy(cpu->memory, font, CH8_FONT_SIZE);

    // Stack sits at offset 0xEA0-0xEFF
    cpu->stack = (u16 *)(cpu->memory + CH8_CALL_STACK_OFFSET);

    // Display refresh sits at 0xF00-0xFFF
    cpu->framebuffer = cpu->memory + CH8_DISPLAY_REFRESH_OFFSET;

    cpu->index = 0;
    cpu->programCounter = CH8_PROGRAM_START_OFFSET;
    cpu->stackPointer = 0;

    cpu->delayTimer = 0;
    cpu->soundTimer = 0;

    cpu->drawFlag = false;
    cpu->waitFlag = false;
    cpu->waitReg = 0;

    ch8_logDebug("CHIP-VM (re)-initialized");
}

void ch8_loadRomData(ch8_cpu *cpu, const u8 *program, size_t size)
{
    assert(cpu != NULL);
    assert(size <= CH8_MAX_PROGRAM_SIZE);

    memset(cpu->memory + CH8_PROGRAM_START_OFFSET, 0, CH8_MAX_PROGRAM_SIZE);
    memcpy(cpu->memory + CH8_PROGRAM_START_OFFSET, program, size);

    ch8_logDebug("%d byte-long ROM binary loaded", size);
}

bool ch8_loadRomFile(ch8_cpu *cpu, const char *file)
{
    assert(cpu != NULL);
    assert(strlen(file) != 0);

    ch8_logDebug("Loading ROM file %s...", file);

    size_t len = 0;
    FILE *f = fopen(file, "rb");
    if (f == NULL)
    {
        ch8_logError("Could not open file %s...", file);
        return false;
    }

    // get file size
    fseek(f, 0, SEEK_END);
    len = ftell(f);
    rewind(f);
    if (len > CH8_MAX_PROGRAM_SIZE)
    {
        ch8_logError("File size too large (%zu bytes)", len);
        return false;
    }

    memset(cpu->memory + CH8_PROGRAM_START_OFFSET, 0, CH8_MAX_PROGRAM_SIZE);
    fread(cpu->memory + CH8_PROGRAM_START_OFFSET, sizeof(u8), len, f);

    fclose(f);

    return true;
}

u16 ch8_nextOpcode(ch8_cpu *cpu)
{
    assert(cpu != NULL);

    u8 msb = cpu->memory[cpu->programCounter];
    u8 lsb = cpu->memory[cpu->programCounter + 1];
    u16 opcode = msb << 8 | lsb;

    return opcode;
}

bool ch8_clockCycle(ch8_cpu *cpu, float elapsed_ms)
{
    assert(cpu != NULL);

    u16 opcode = ch8_nextOpcode(cpu);
    if (opcode == 0) {
        return false;
    }

    // Set flags to false before each instruction
    cpu->drawFlag = false;
    cpu->waitFlag = false;
    cpu->waitReg = 0;

    switch (opcode & 0xF000)
    {
    // Call a machine code subroutine
    case 0x0000:
        switch (opcode & 0x00FF)
        {
        case 0x00E0:
            ch8_op_ClearDisplay(cpu);
            break;
        case 0x00EE:
            ch8_op_ReturnFromSub(cpu);
            break;
        default:
            ch8_logDebug("NOOP");
            break;
        }
        break;
    // Jump to adresss
    case 0x1000:
        ch8_op_JumpTo(cpu, opcode);
        break;
    // Call ROM subroutine
    case 0x2000:
        ch8_op_CallSub(cpu, opcode);
        break;
    // Equality check
    case 0x3000:
        ch8_op_SkipEquals(cpu, opcode);
        break;
    // Negated Equality check
    case 0x4000:
        ch8_op_SkipNotEquals(cpu, opcode);
        break;
    // Equality check against two variables
    case 0x5000:
        ch8_op_SkipVXEqualsVY(cpu, opcode);
        break;
    // Set constant in register
    case 0x6000:
        ch8_op_Set(cpu, opcode);
        break;
    // Add constant to register value
    case 0x7000:
        ch8_op_Add(cpu, opcode);
        break;
    // Arithmetic
    case 0x8000:
    {
        switch (opcode & 0x000F)
        {
        // Assign value of register B to register A
        case 0x0000:
            ch8_op_Assign(cpu, opcode);
            break;
        case 0x0001:
            ch8_op_LogicalOr(cpu, opcode);
            break;
        case 0x0002:
            ch8_op_LogicalAnd(cpu, opcode);
            break;
        case 0x0003:
            ch8_op_LogicalXor(cpu, opcode);
            break;
        case 0x0004:
            ch8_op_AddAssign(cpu, opcode);
            break;
        case 0x0005:
            ch8_op_SubtractAssign(cpu, opcode);
            break;
        case 0x0006:
            ch8_op_BitshiftRight(cpu, opcode);
            break;
        case 0x0007:
            ch8_op_SubtractAssignReverse(cpu, opcode);
            break;
        case 0x000E:
            ch8_op_BitshiftLeft(cpu, opcode);
            break;
        default:
            ch8_logError("Invalid opcode %X", opcode);
            break;
        }
        break;
    }
    // Negated equality check against two variables
    case 0x9000:
        ch8_op_SkipVXNotEqualsVY(cpu, opcode);
        break;
    // Set memory address
    case 0xA000:
        ch8_op_SetIndex(cpu, opcode);
        break;
    case 0xB000:
        ch8_op_JumpOffset(cpu, opcode);
        break;
    case 0xC000:
        ch8_op_BitwiseRandom(cpu, opcode);
        break;
    case 0xD000:
        ch8_op_DrawSprite(cpu, opcode);
        break;
    case 0xE000:
    {
        switch (opcode & 0x00FF)
        {
        case 0x009E:
            ch8_op_KeyEquals(cpu, opcode);
            break;
        case 0x00A1:
            ch8_op_KeyNotEquals(cpu, opcode);
            break;
        default:
            ch8_logError("Invalid opcode %X", opcode);
            break;
        }
        break;
    }
    case 0xF000:
    {
        switch (opcode & 0x00FF)
        {
        case 0x0007:
            ch8_op_ReadDelayTimer(cpu, opcode);
            break;
        case 0x000A:
            ch8_op_KeyWait(cpu, opcode);
            break;
        case 0x0015:
            ch8_op_SetDelayTimer(cpu, opcode);
            break;
        case 0x0018:
            ch8_op_SetSoundTimer(cpu, opcode);
            break;
        case 0x001E:
            ch8_op_AddToIndex(cpu, opcode);
            break;
        case 0x0029:
            ch8_op_SetFontChar(cpu, opcode);
            break;
        case 0x0033:
            ch8_op_StoreBinaryCodedDecimal(cpu, opcode);
            break;
        case 0x0055:
            ch8_op_Store(cpu, opcode);
            break;
        case 0x0065:
            ch8_op_Load(cpu, opcode);
            break;
        default:
            ch8_logError("Invalid opcode %X", opcode);
            break;
        }
        break;
    }
    default:
        ch8_logError("Unrecognized opcode: %X", opcode);
        return false;
    }

    return true;
}

bool ch8_getPixel(const ch8_cpu *cpu, int x, int y)
{
    int index = y * CH8_DISPLAY_WIDTH + x;
    int byteIndex = index / 8;
    int offset = index % 8;
    return cpu->framebuffer[byteIndex] & (0x80 >> offset);
}

void ch8_setPixel(ch8_cpu *cpu, int x, int y, bool on)
{
    int index = y * CH8_DISPLAY_WIDTH + x;
    int byteIndex = index / 8;
    int offset = index % 8;
    u8 byte = cpu->framebuffer[byteIndex];

    if(on) {
        byte = byte | (0x80 >> offset);
    } else {
        byte = byte & (~(0x80 >> offset));
    }

    cpu->framebuffer[byteIndex] = byte;
}
