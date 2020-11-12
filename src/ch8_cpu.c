#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "ch8_cpu.h"
#include "display.h"
#include "util.h"

void display_clear(ch8_cpu *cpu)
{
    begin_frame(cpu->display);
    memset(cpu->display, 0, CH8_DISPLAY_SIZE);
    end_frame();
}

void ch8_init(ch8_cpu **pcpu)
{
    assert(pcpu != NULL);

    ch8_cpu *cpu = (ch8_cpu *)ch8_malloc(sizeof(ch8_cpu));
    (*pcpu) = cpu;

    ch8_reset(cpu);
}

void ch8_quit(ch8_cpu **pcpu)
{
    assert(pcpu != NULL);
    ch8_free((void **)pcpu);
}

void ch8_reset(ch8_cpu *cpu)
{
    assert(cpu != NULL);

    memset(cpu->memory, 0, CH8_MEM_SIZE);
    memset(cpu->V, 0, CH8_NUM_REGISTERS);
    memset(cpu->stack, 0, CH8_STACK_SIZE);
    memset(cpu->display, 0, CH8_DISPLAY_SIZE);

    cpu->I = 0;
    cpu->PC = CH8_PROGRAM_START_OFFSET;
    cpu->running = true;
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
        printf("Could not open file %s...\n", file);
        return false;
    }

    // get file size
    fseek(f, 0, SEEK_END);
    len = ftell(f);
    rewind(f);
    if (len > CH8_MAX_PROGRAM_SIZE)
    {
        printf("File size too large (%zu bytes)\n", len);
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
        return false;
    }
    else
    {
        cpu->PC += 2;
    }

    switch (opcode & 0xF000)
    {
    case CH8_OPCODE_MSB_CALL:
        switch (opcode & 0x00FF)
        {
        case CH8_OPCODE_DISPLAY_CLEAR:
            display_clear(cpu);
            break;

        case CH8_OPCODE_RETURN:
            printf("Return from subroutine\n");
            break;
        }
        break;
    default:
        printf("Unrecognized opcode: %X\n", opcode);
        return false;
    }

    return true;
}
