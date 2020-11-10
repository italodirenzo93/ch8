#include "ch8_cpu.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>

void ch8_init(ch8_cpu **pcpu)
{
    ch8_cpu *cpu = (ch8_cpu *)malloc(sizeof(ch8_cpu));
    memset(cpu, 0, sizeof(ch8_cpu));

    cpu->PC = PROGRAM_START_OFFSET;

    (*pcpu) = cpu;
}

void ch8_quit(ch8_cpu *cpu)
{
    free(cpu);
}

void ch8_load_rom(ch8_cpu *cpu, uint8_t *program, size_t size)
{
    assert(size <= MAX_PROGRAM_SIZE);
    memcpy(cpu->memory + PROGRAM_START_OFFSET, program, size);
}

bool ch8_load_rom_file(ch8_cpu *cpu, const char *file)
{
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
    if (len > MAX_PROGRAM_SIZE)
    {
        printf("File size too large (%d bytes)\n", len);
        return false;
    }

    fread(cpu->memory + PROGRAM_START_OFFSET, sizeof(uint8_t), len, f);

    fclose(f);

    return true;
}

uint16_t ch8_next_opcode(ch8_cpu *cpu)
{
    uint8_t msb = cpu->memory[cpu->PC];
    uint8_t lsb = cpu->memory[cpu->PC + 1];
    uint16_t opcode = msb << 8 | lsb;

    cpu->PC += 2;

    return opcode;
}

void ch8_exec_opcode(ch8_cpu *cpu, uint16_t opcode)
{
    switch (opcode & 0xF000)
    {
    case CH8_OPCODE_MSB_CALL:
        switch (opcode & 0x00FF)
        {
        case CH8_OPCODE_DISPLAY_CLEAR:
            puts("Display Clear");
            break;

        case CH8_OPCODE_RETURN:
            puts("Return from subroutine");
            break;
        }
        break;
    }
}
