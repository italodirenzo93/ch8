#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include "ch8_cpu.h"
#include "opcodes.h"
#include "log.h"
#include "util.h"

// clang-format off
static const uint8_t font[] = {
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

#define FONT_SIZE 80
// clang-format on

void ch8_reset(ch8_cpu *cpu)
{
    assert(cpu != NULL);
    
    int i;

    // Memory
    for (i = 0; i < CH8_MEM_SIZE; i++) {
        cpu->memory[i] = i < FONT_SIZE ? font[i] : 0;
    }

    // Data registers
    for (i = 0; i < CH8_NUM_REGISTERS; i++) {
        cpu->V[i] = 0;
    }

    // Stack
    for (i = 0; i < CH8_STACK_SIZE; i++) {
        cpu->stack[i] = 0;
    }

    // Framebuffer
    const int display_size = CH8_DISPLAY_WIDTH * CH8_DISPLAY_HEIGHT;
    for (i = 0; i < display_size; i++) {
        cpu->framebuffer[i] = CH8_PIXEL_OFF;
    }

    // Keypad
    for (i = 0; i < CH8_NUM_KEYS; i++) {
        cpu->keypad[i] = CH8_KEYSTATE_DOWN;
    }

    cpu->index_register = 0;
    cpu->program_counter = CH8_PROGRAM_START_OFFSET;
    cpu->stack_pointer = 0;
    cpu->running = false;

    cpu->delay_timer = 0;
    cpu->sound_timer = 0;

    log_debug("CHIP-VM (re)-initialized");
}

void ch8_load_rom(ch8_cpu *cpu, const uint8_t *program, size_t size)
{
    assert(cpu != NULL);
    assert(size <= CH8_MAX_PROGRAM_SIZE);

    const int length = CH8_MAX_PROGRAM_SIZE - CH8_PROGRAM_START_OFFSET;
    for (int i = 0; i < length; i++) {
        cpu->memory[CH8_PROGRAM_START_OFFSET + i] = i < size ? program[i] : 0;
    }

    log_debug("%d byte-long ROM binary loaded", size);
}

bool ch8_load_rom_file(ch8_cpu *cpu, const char *file)
{
    assert(cpu != NULL);
    assert(strlen(file) != 0);

    log_debug("Loading ROM file %s...", file);

    size_t len = 0;
    FILE *f = fopen(file, "rb");
    if (f == NULL)
    {
        log_error("Could not open file %s...", file);
        return false;
    }

    // get file size
    fseek(f, 0, SEEK_END);
    len = ftell(f);
    rewind(f);
    if (len > CH8_MAX_PROGRAM_SIZE)
    {
        log_error("File size too large (%zu bytes)", len);
        return false;
    }

    memset(cpu->memory + CH8_PROGRAM_START_OFFSET, 0, CH8_MAX_PROGRAM_SIZE);
    fread(cpu->memory + CH8_PROGRAM_START_OFFSET, sizeof(uint8_t), len, f);

    fclose(f);

    return true;
}

uint16_t ch8_next_opcode(ch8_cpu *cpu)
{
    assert(cpu != NULL);

    uint8_t msb = cpu->memory[cpu->program_counter];
    uint8_t lsb = cpu->memory[cpu->program_counter + 1];
    uint16_t opcode = msb << 8 | lsb;

    return opcode;
}

bool ch8_clock_cycle(ch8_cpu *cpu)
{
    assert(cpu != NULL);

    if (!cpu->running)
    {
        log_error("Could not execute opcode because CHIP-8 VM is not running");
        return false;
    }

    const uint16_t opcode = ch8_next_opcode(cpu);
    if (opcode == 0)
    {
        cpu->running = false;
        log_debug("ROM exited");
        return false;
    }

    switch (opcode & 0xF000)
    {
    // Call a machine code subroutine
    case 0x0000:
        switch (opcode & 0x00FF)
        {
        case 0x00E0:
            ch8_op_display_clear(cpu);
            break;
        case 0x00EE:
            ch8_op_return(cpu);
            break;
        default:
            log_debug("NOOP");
            break;
        }
        break;
    // Jump to adresss
    case 0x1000:
        ch8_op_jumpto(cpu, opcode);
        break;
    // Call ROM subroutine
    case 0x2000:
        ch8_op_callsub(cpu, opcode);
        break;
    // Equality check
    case 0x3000:
        ch8_op_cond_eq(cpu, opcode);
        break;
    // Negated Equality check
    case 0x4000:
        ch8_op_cond_neq(cpu, opcode);
        break;
    // Equality check against two variables
    case 0x5000:
        ch8_op_cond_vx_eq_vy(cpu, opcode);
        break;
    // Set constant in register
    case 0x6000:
        ch8_op_const_set(cpu, opcode);
        break;
    // Add constant to register value
    case 0x7000:
        ch8_op_const_add(cpu, opcode);
        break;
    // Arithmetic
    case 0x8000:
    {
        switch (opcode & 0x000F)
        {
        // Assign value of register B to register A
        default:
        case 0x0000:
            ch8_op_assign(cpu, opcode);
            break;
        case 0x0001:
            ch8_op_or(cpu, opcode);
            break;
        case 0x0002:
            ch8_op_and(cpu, opcode);
            break;
        case 0x0003:
            ch8_op_xor(cpu, opcode);
            break;
        case 0x0004:
            ch8_op_add_vx_to_vy(cpu, opcode);
            break;
        case 0x0005:
            ch8_op_sub_vy_from_vx(cpu, opcode);
            break;
        case 0x0006:
            ch8_op_bitshift_right_vx_to_vf(cpu, opcode);
            break;
        case 0x0007:
            ch8_op_set_vx_to_vx_sub_vy(cpu, opcode);
            break;
        case 0x000E:
            ch8_op_bitshift_left_vx_to_vf(cpu, opcode);
            break;
        }
        break;
    }
    // Negated equality check against two variables
    case 0x9000:
        ch8_op_cond_vx_neq_vy(cpu, opcode);
        break;
    // Set memory address
    case 0xA000:
        ch8_op_set_addr(cpu, opcode);
        break;
    case 0xB000:
        ch8_jump_to_addr_plus_v0(cpu, opcode);
        break;
    case 0xC000:
        ch8_op_bitwise_rand(cpu, opcode);
        break;
    case 0xD000:
        ch8_op_draw_sprite(cpu, opcode);
        break;
    case 0xE000:
    {
        uint8_t vx = (opcode & 0x0F00) >> 8;
        switch (opcode & 0x00FF)
        {
        case 0x009E:
            ch8_op_keyop_eq(cpu, vx);
            break;
        case 0x00A1:
            ch8_op_keyop_neq(cpu, vx);
            break;
        }
        break;
    }
    case 0xF000:
    {
        switch (opcode & 0x00FF)
        {
            case 0x0007:
                ch8_op_set_vx_to_delay_timer(cpu, opcode);
                break;
            case 0x000A:
                ch8_op_await_keypress(cpu, opcode);
                break;
            case 0x0015:
                ch8_op_set_delay_timer_to_vx(cpu, opcode);
                break;
            case 0x0018:
                ch8_op_set_sound_timer_to_vx(cpu, opcode);
                break;
            case 0x001E:
                ch8_op_add_vx_to_I(cpu, opcode);
                break;
            case 0x0029:
                ch8_op_set_I_to_sprite_addr(cpu, opcode);
                break;
            case 0x0033:
                ch8_op_store_bcd_of_vx(cpu, opcode);
                break;
            case 0x0055:
                ch8_op_store_v0_to_vx(cpu, opcode);
                break;
            case 0x0065:
                ch8_op_fill_v0_to_vx(cpu, opcode);
                break;
        }
        break;
    }
    default:
        log_error("Unrecognized opcode: %X", opcode);
        return false;
    }

    // Advance the program counter 2 bytes at a time
    cpu->program_counter += CH8_PC_STEP_SIZE;

    return true;
}

static float timer_val = 0.0f;
void ch8_update_timers(ch8_cpu *cpu, float elapsed)
{
    assert(cpu != NULL);
    
    timer_val += elapsed;

    if (timer_val >= 16.666f) {
        cpu->delay_timer = ch8_max(0, cpu->delay_timer - 1);
        cpu->sound_timer = ch8_max(0, cpu->sound_timer - 1);

        timer_val = 0.0f;
    }
}
