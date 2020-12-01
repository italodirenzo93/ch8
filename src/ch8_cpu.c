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

static float delay_timer_val = 0.0f;
static float sound_timer_val = 0.0f;

static void _update_timer(uint8_t *timer, float *accumulator, float elapsed_ms)
{
    assert(timer != NULL);
    assert(accumulator != NULL);
    if (*timer != 0) {
        *accumulator += elapsed_ms;
        if (*accumulator >= 16.666f) {
            *timer -= 1;
            *accumulator = 0.0f;
            printf("Accum: %f\n", *accumulator);
        }
    }
}

void ch8_reset(ch8_cpu *cpu)
{
    assert(cpu != NULL);

    // Initialize all memory to 0
    memset(cpu->memory, 0, CH8_MEM_SIZE);

    // Font data sits at beginning
    memcpy(cpu->memory, font, FONT_SIZE);

    // Stack sits at offset 0xEA0-0xEFF
    cpu->stack = (uint16_t *)(cpu->memory + 0xEA0);

    // Display refresh sits at 0xF00-0xFFF
    cpu->framebuffer = (cpu->memory + 0xF00);

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

    memset(cpu->memory + CH8_PROGRAM_START_OFFSET, 0, CH8_MAX_PROGRAM_SIZE);
    memcpy(cpu->memory + CH8_PROGRAM_START_OFFSET, program, size);

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

bool ch8_clock_cycle(ch8_cpu *cpu, float elapsed_ms)
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
        default:
            log_error("Invalid opcode %X", opcode);
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
        switch (opcode & 0x00FF)
        {
        case 0x009E:
            ch8_op_keyop_eq(cpu, opcode);
            break;
        case 0x00A1:
            ch8_op_keyop_neq(cpu, opcode);
            break;
        default:
            log_error("Invalid opcode %X", opcode);
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
        default:
            log_error("Invalid opcode %X", opcode);
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

    // Update timers
    _update_timer(&cpu->delay_timer, &delay_timer_val, elapsed_ms);
    _update_timer(&cpu->sound_timer, &sound_timer_val, elapsed_ms);

    return true;
}

bool ch8_get_pixel(const ch8_cpu *cpu, int x, int y)
{
    int index = y * CH8_DISPLAY_WIDTH + x;
    int byteIndex = index / 8;
    int offset = index % 8;
    return cpu->framebuffer[byteIndex] & (0x80 >> offset);
}

void ch8_set_pixel(ch8_cpu *cpu, int x, int y, bool on)
{
    int index = y * CH8_DISPLAY_WIDTH + x;
    int byteIndex = index / 8;
    int offset = index % 8;
    uint8_t byte = cpu->framebuffer[byteIndex];

    if(on) {
        byte = byte | (0x80 >> offset);
    } else {
        byte = byte & (~(0x80 >> offset));
    }

    cpu->framebuffer[byteIndex] = byte;
}
