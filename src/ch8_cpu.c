#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include "ch8_cpu.h"
#include "opcodes.h"

void ch8_init(ch8_cpu **pcpu)
{
    assert(pcpu != NULL);

    ch8_cpu *cpu = (ch8_cpu *)malloc(sizeof(ch8_cpu));
    memset(cpu, 0, sizeof(ch8_cpu));
    (*pcpu) = cpu;

    ch8_reset(cpu);

    srand((unsigned int)time(NULL));
}

void ch8_quit(ch8_cpu **pcpu)
{
    assert(pcpu != NULL);
    ch8_cpu *cpu = *pcpu;
    if (cpu != NULL)
    {
        free(cpu);
        cpu = NULL;
    }
}

void ch8_reset(ch8_cpu *cpu)
{
    assert(cpu != NULL);

    memset(cpu->memory, 0, CH8_MEM_SIZE);
    memset(cpu->V, 0, CH8_NUM_REGISTERS);
    memset(cpu->stack, 0, CH8_STACK_SIZE);

    cpu->I = 0;
    cpu->PC = CH8_PROGRAM_START_OFFSET;
    cpu->running = true;

    cpu->delayTimer = 0;
    cpu->soundTimer = 0;
}

void ch8_load_rom(ch8_cpu *cpu, uint8_t *program, size_t size)
{
    assert(cpu != NULL);
    assert(size <= CH8_MAX_PROGRAM_SIZE);
    memcpy(cpu->memory + CH8_PROGRAM_START_OFFSET, program, size);
}

bool ch8_load_rom_file(ch8_cpu *cpu, const char *file)
{
    assert(cpu != NULL);

    size_t len = 0;
    FILE *f = fopen(file, "rb");
    if (f == NULL)
    {
        fprintf(stderr, "Could not open file %s...\n", file);
        return false;
    }

    // get file size
    fseek(f, 0, SEEK_END);
    len = ftell(f);
    rewind(f);
    if (len > CH8_MAX_PROGRAM_SIZE)
    {
        fprintf(stderr, "File size too large (%zu bytes)\n", len);
        return false;
    }

    fread(cpu->memory + CH8_PROGRAM_START_OFFSET, sizeof(uint8_t), len, f);

    fclose(f);

    return true;
}

uint16_t ch8_next_opcode(ch8_cpu *cpu)
{
    assert(cpu != NULL);

    uint8_t msb = cpu->memory[cpu->PC];
    uint8_t lsb = cpu->memory[cpu->PC + 1];
    uint16_t opcode = msb << 8 | lsb;

    return opcode;
}

bool ch8_exec_opcode(ch8_cpu *cpu)
{
    assert(cpu != NULL);

    if (!cpu->running)
        return false;

    uint16_t opcode = ch8_next_opcode(cpu);
    if (opcode == 0)
    {
        cpu->running = false;
        printf("ROM exited\n");
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
            printf("Call machine code routine at address\n");
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
            break;
        case 0x0007:
            break;
        case 0x000E:
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
    default:
        printf("Unrecognized opcode: %X\n", opcode);
        return false;
    }

    // Advance the program counter 2 bytes at a time
    cpu->PC += 2;

    return true;
}
