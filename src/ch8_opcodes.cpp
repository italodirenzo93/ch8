#include "ch8_opcodes.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "ch8_log.h"
#include "ch8_util.h"

static inline void next(ch8_cpu* cpu)
{
    cpu->programCounter += CH8_PC_STEP_SIZE;
}

// 0x00E0
void ch8_op_ClearDisplay(ch8_cpu *cpu)
{
    assert(cpu != NULL);
    ch8_logDebug("[00E0] - Clear Display");

    for (int i = 0; i < CH8_DISPLAY_SIZE; i++) {
        cpu->framebuffer[i] = 0;
    }

    next(cpu);
}

// 0x00EE
void ch8_op_ReturnFromSub(ch8_cpu* cpu)
{
    assert(cpu != NULL);
    ch8_logDebug("[00EE] - Return from sub-routine");

    // Set program counter to address at top of stack
    cpu->programCounter = cpu->stack[--cpu->stackPointer];

    next(cpu);
}

// 0x1NNN
void ch8_op_JumpTo(ch8_cpu* cpu, u16 opcode)
{
    assert(cpu != NULL);
    u16 addr = opcode & 0x0FFF;

    ch8_logDebug("[1NNN] - Jump to address %X", addr);

    cpu->programCounter = addr;
}

// 0x2NNN
void ch8_op_CallSub(ch8_cpu *cpu, u16 opcode)
{
    assert(cpu != NULL);

    u16 addr = opcode & 0x0FFF;
    ch8_logDebug("[2NNN] - Call subroutine at address %X", addr);

    // TODO: check for stack overflow

    cpu->stack[cpu->stackPointer++] = cpu->programCounter;
    cpu->programCounter = addr;
}

// 0x3XKK
void ch8_op_SkipEquals(ch8_cpu *cpu, u16 opcode)
{
    assert(cpu != NULL);
    u8 x = (opcode & 0x0F00) >> 8;
    u8 operand = opcode & 0x00FF;

    ch8_logDebug("[3XKK] - IF conditional (V[%d] == %d)", x, operand);

    if (cpu->V[x] == operand) {
        // Skip the next instruction
        next(cpu);
    }

    next(cpu);
}

// 0x4XKK
void ch8_op_SkipNotEquals(ch8_cpu *cpu, u16 opcode)
{
    assert(cpu != NULL);
    u8 x = (opcode & 0x0F00) >> 8;
    u8 operand = opcode & 0x00FF;

    ch8_logDebug("[4XKK] - IF conditional (V[%d] != %d)", x, operand);

    if (cpu->V[x] != operand) {
        next(cpu);
    }

    next(cpu);
}

// 0x5XY0
void ch8_op_SkipVXEqualsVY(ch8_cpu *cpu, u16 opcode)
{
    assert(cpu != NULL);

    u8 x = (opcode & 0x0F00) >> 8;
    u8 y = (opcode & 0x00F0) >> 4;

    ch8_logDebug("[5XY0] - IF conditional (V[%d] == V[%d])\n", x, y);

    if (cpu->V[x] == cpu->V[y]) {
        next(cpu);
    }

    next(cpu);
}

// 0x6XKK
void ch8_op_Set(ch8_cpu *cpu, u16 opcode)
{
    assert(cpu != NULL);

    u8 x = (opcode & 0x0F00) >> 8;
    u8 value = opcode & 0x00FF;

    ch8_logDebug("[6XKK] - SET register V[%d] = %d\n", x, value);

    cpu->V[x] = value;

    next(cpu);
}

// 0x7XKK
void ch8_op_Add(ch8_cpu *cpu, u16 opcode)
{
    assert(cpu != NULL);

    u8 x = (opcode & 0x0F00) >> 8;
    u8 operand = opcode & 0x00FF;

    ch8_logDebug("[7XKK] - ADD %d to register V[%d]\n", operand, x);

    cpu->V[x] += operand;

    next(cpu);
}

// 0x8XY0
void ch8_op_Assign(ch8_cpu *cpu, u16 opcode)
{
    assert(cpu != NULL);

    u8 x = (opcode & 0x0F00) >> 8;
    u8 y = (opcode & 0x00F0) >> 4;

    ch8_logDebug("[8XY0] - SET V[%d] = V[%d]\n", x, y);

    cpu->V[x] = cpu->V[y];

    next(cpu);
}

// 0x8XY1
void ch8_op_LogicalOr(ch8_cpu *cpu, u16 opcode)
{
    assert(cpu != NULL);

    u8 x = (opcode & 0x0F00) >> 8;
    u8 y = (opcode & 0x00F0) >> 4;

    ch8_logDebug("[8XY1] - OR V[%d] = (V[%d] | V[%d])\n", x, x, y);

    cpu->V[x] |= cpu->V[y];

    next(cpu);
}

// 0x8XY2
void ch8_op_LogicalAnd(ch8_cpu *cpu, u16 opcode)
{
    assert(cpu != NULL);

    u8 x = (opcode & 0x0F00) >> 8;
    u8 y = (opcode & 0x00F0) >> 4;

    ch8_logDebug("[8XY2] - AND V[%d] = (V[%d] & V[%d])\n", x, x, y);

    cpu->V[x] &= cpu->V[y];

    next(cpu);
}

// 0x8XY3
void ch8_op_LogicalXor(ch8_cpu *cpu, u16 opcode)
{
    assert(cpu != NULL);

    u8 x = (opcode & 0x0F00) >> 8;
    u8 y = (opcode & 0x00F0) >> 4;

    ch8_logDebug("[8XY3] - XOR V[%d] = (V[%d] ^ V[%d])\n", x, x, y);

    cpu->V[x] ^= cpu->V[y];

    next(cpu);
}

// 0x8XY4
void ch8_op_AddAssign(ch8_cpu *cpu, u16 opcode)
{
    assert(cpu != NULL);

    u8 x = (opcode & 0x0F00) >> 8;
    u8 y = (opcode & 0x00F0) >> 4;

    u16 sum = cpu->V[x] + cpu->V[y];

    //set carry flag
    cpu->V[0xF] = 0;
    if (sum > UINT8_MAX) {
        cpu->V[0xF] = 1;
    }

    cpu->V[x] = (u8)sum;

    ch8_logDebug("[8XY4] - ADD V[%d] += V[%d] : V[0xF] = %d", x, y, cpu->V[0xF]);

    next(cpu);
}

// 0x8XY5
void ch8_op_SubtractAssign(ch8_cpu *cpu, u16 opcode)
{
    assert(cpu != NULL);

    u8 x = (opcode & 0x0F00) >> 8;
    u8 y = (opcode & 0x00F0) >> 4;

    cpu->V[0xF] = 0;

    if (cpu->V[x] > cpu->V[y]) {
        cpu->V[0xF] = 1;
    }

    cpu->V[x] = cpu->V[x] - cpu->V[y];

    ch8_logDebug("[8XY5] - SUB V[%d] -= V[%d] : V[0xF] = %d", x, y, cpu->V[0xF]);

    next(cpu);
}

// 0x8XY6
void ch8_op_BitshiftRight(ch8_cpu *cpu, u16 opcode)
{
    assert(cpu != NULL);

    u8 x = (opcode & 0x0F00) >> 8;
    u8 y = (opcode & 0x00F0) >> 4;

    cpu->V[0xF] = cpu->V[x] & 0x1;
    cpu->V[x] = cpu->V[y] >> 1;

    ch8_logDebug("[8XY6] - SHIFTR V[%d] >>= 1", x);

    next(cpu);
}

// 0x8XY7
void ch8_op_SubtractAssignReverse(ch8_cpu *cpu, u16 opcode)
{
    assert(cpu != NULL);

    u8 x = (opcode & 0x0F00) >> 8;
    u8 y = (opcode & 0x00F0) >> 4;

    u8 vx = cpu->V[x];
    u8 vy = cpu->V[y];

    cpu->V[x] = vy - vx;
    cpu->V[0xF] = vy < vx ? 0x0 : 0x1;

    ch8_logDebug("[8XY7] - SUB V[%d] = V[%d] - V[%d]", x, y, x);

    next(cpu);
}

// 0x8XYE
void ch8_op_BitshiftLeft(ch8_cpu *cpu, u16 opcode)
{
    assert(cpu != NULL);

    u8 x = (opcode & 0x0F00) >> 8;
    u8 y = (opcode & 0x00F0) >> 4;

    cpu->V[0xF] = (cpu->V[x] >> 7) & 0x1;
    cpu->V[x] = cpu->V[y] << 1;

    ch8_logDebug("[8XYE] - SHIFTL V[%d] <<= 1", x);

    next(cpu);
}

// 0x9XY0
void ch8_op_SkipVXNotEqualsVY(ch8_cpu *cpu, u16 opcode)
{
    assert(cpu != NULL);

    u8 x = (opcode & 0x0F00) >> 8;
    u8 y = (opcode & 0x00F0) >> 4;

    ch8_logDebug("[9XY0] - IF conditional (V[%d] != V[%d])\n", x, y);

    if (cpu->V[x] != cpu->V[y]) {
        next(cpu);
    }

    next(cpu);
}

// 0xANNN
void ch8_op_SetIndex(ch8_cpu *cpu, u16 opcode)
{
    assert(cpu != NULL);

    u16 addr = opcode & 0x0FFF;

    ch8_logDebug("[ANNN] - SET I = %d\n", addr);

    cpu->index = addr;

    next(cpu);
}

// 0xBNNN
void ch8_op_JumpOffset(ch8_cpu *cpu, u16 opcode)
{
    assert(cpu != NULL);

    u8 x = (opcode & 0x0F00) >> 8;
    u8 addr = opcode & 0x00FF;

    ch8_logDebug("JUMP to %d + V[%d]\n", x, addr);

    cpu->programCounter = addr + cpu->V[x];
}

// 0xCNNN
void ch8_op_BitwiseRandom(ch8_cpu *cpu, u16 opcode)
{
    assert(cpu != NULL);
    u8 x = (opcode & 0x0F00) >> 8;
    u8 nn = (opcode & 0x00FF);

    ch8_logDebug("RAND V[%d] = rand() & %d\n", x, nn);

    cpu->V[x] = ch8_randU8() & nn;

    next(cpu);
}

// 0xDXYN
void ch8_op_DrawSprite(ch8_cpu *cpu, u16 opcode)
{
    assert(cpu != NULL);

    u8 x = (opcode & 0x0F00) >> 8;
    u8 y = (opcode & 0x00F0) >> 4;
    u8 n = opcode & 0x000F;

    int startX = cpu->V[x];
    int startY = cpu->V[y];

    if (startX >= CH8_DISPLAY_WIDTH) {
        startX = startX % CH8_DISPLAY_WIDTH;
    }
    if (startY >= CH8_DISPLAY_HEIGHT) {
        startY = startY % CH8_DISPLAY_HEIGHT;
    }

    int endX = ch8_min(startX + 8, CH8_DISPLAY_WIDTH);
    int endY = ch8_min(startY + n, CH8_DISPLAY_HEIGHT);

    cpu->V[0xF] = 0;

    for (int y = startY; y < endY; y++) {
        u8 spriteByte = cpu->memory[cpu->index + (y - startY)];
        for (int x = startX; x < endX; x++) {
            // NOTE: spritePixel and screenPixel are 0 or non-zero
            // not 0 or 1 !!!
            int spritePixel = spriteByte & (0x80 >> (x - startX));
            int screenPixel = ch8_getPixel(cpu, x, y);

            if(spritePixel) {
                if(screenPixel) {
                    cpu->V[0xF] = 1;
                }

                ch8_setPixel(cpu, x, y, screenPixel == 0 ? true : false);
            }
        }
    }

    // Set the flag indicating that the framebuffer should be drawn to the screen
    cpu->drawFlag = true;

    ch8_logDebug("[DXYN] - Draw X: %d, Y: %d, N: %d", cpu->V[x], cpu->V[y], n);

    next(cpu);
}

// 0xEX9E
void ch8_op_KeyEquals(ch8_cpu *cpu, u16 opcode)
{
    assert(cpu != NULL);

    u8 key = (opcode & 0x0F00) >> 8;

    ch8_logDebug("KEY check if keypad[%d] is down\n", key);

    if (cpu->keypad[key] == true) {
        next(cpu);
    }

    next(cpu);
}

// 0xEXA1
void ch8_op_KeyNotEquals(ch8_cpu *cpu, u16 opcode)
{
    assert(cpu != NULL);

    u8 key = (opcode & 0x0F00) >> 8;

    ch8_logDebug("KEY check if keypad[%d] is up\n", key);

    if (cpu->keypad[key] == false) {
        next(cpu);
    }

    next(cpu);
}

// 0xFX07
void ch8_op_ReadDelayTimer(ch8_cpu *cpu, u16 opcode)
{
    assert(cpu != NULL);

    u16 x = (opcode & 0x0F00) >> 8;

    cpu->V[x] = cpu->delayTimer;

    ch8_logDebug("TIMER set V[%d] = delay timer val %d", x, cpu->delayTimer);

    next(cpu);
}

// 0xFX0A
void ch8_op_KeyWait(ch8_cpu *cpu, u16 opcode)
{
    assert(cpu != NULL);

    ch8_logDebug("[FX0A] - Await keypress...");

    cpu->waitFlag = true;
    cpu->waitReg = (opcode & 0x0F00) >> 8;

    next(cpu);
}

// 0xFX15
void ch8_op_SetDelayTimer(ch8_cpu *cpu, u16 opcode)
{
    assert(cpu != NULL);

    u16 x = (opcode & 0x0F00) >> 8;

    cpu->delayTimer = cpu->V[x];

    ch8_logDebug("TIMER set delay timer to V[%d] (%d)", x, cpu->delayTimer);

    next(cpu);
}

// 0xFX18
void ch8_op_SetSoundTimer(ch8_cpu *cpu, u16 opcode)
{
    assert(cpu != NULL);

    u16 x = (opcode & 0x0F00) >> 8;

    cpu->soundTimer = cpu->V[x];

    ch8_logDebug("SOUND set sound timer to V[%d] (%d)", x, cpu->soundTimer);

    next(cpu);
}

// 0xFX1E
void ch8_op_AddToIndex(ch8_cpu *cpu, u16 opcode)
{
    assert(cpu != NULL);

    u16 x = (opcode & 0x0F00) >> 8;

    cpu->index += cpu->V[x];

    ch8_logDebug("[FX1E] - ADD I += V[%d]", x);

    next(cpu);
}

// 0xFX29
void ch8_op_SetFontChar(ch8_cpu *cpu, u16 opcode)
{
    assert(cpu != NULL);

    u8 x = (opcode & 0x0F00) >> 8;

    cpu->index = cpu->V[x] * 5;

    ch8_logDebug("[FX29] - SET I = V[%d] * 5 (sprite_addr)", x);

    next(cpu);
}

// 0xFX33
void ch8_op_StoreBinaryCodedDecimal(ch8_cpu *cpu, u16 opcode)
{
    assert(cpu != NULL);

    u8 x = (opcode & 0x0F00) >> 8;

    cpu->memory[cpu->index] = (u8)cpu->V[x] / 100;
    cpu->memory[cpu->index + 1] = (u8)(cpu->V[x] % 100) / 10;
    cpu->memory[cpu->index + 2] = (u8)cpu->V[x] % 10;

    ch8_logDebug("[FX33] - BCD store V[%d] (%d)", x, cpu->V[x]);

    next(cpu);
}

// 0xFX55
void ch8_op_Store(ch8_cpu *cpu, u16 opcode)
{
    assert(cpu != NULL);

    u8 x = (opcode & 0x0F00) >> 8;

    for (int i = 0; i <= x; i++) {
        cpu->memory[cpu->index + i] = cpu->V[i];
    }

    //cpu->index += x + 1;

    ch8_logDebug("[FX55] - STORE V[0]..V[%d] in memory", x);

    next(cpu);
}

// 0xFX65
void ch8_op_Load(ch8_cpu *cpu, u16 opcode)
{
    assert(cpu != NULL);

    u8 x = (opcode & 0x0F00) >> 8;

    for (int i = 0; i <= x; i++) {
        cpu->V[i] = cpu->memory[cpu->index + i];
    }

    //cpu->index += x + 1;

    ch8_logDebug("[FX55] - FILL memory bytes into V[0]..V[%d]", x);

    next(cpu);
}
