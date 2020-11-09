#include "ch8_cpu.h"

#include <assert.h>
#include <string.h>

ch8_cpu *ch8_init()
{
    ch8_cpu *cpu = (ch8_cpu *)malloc(sizeof(ch8_cpu));
    memset(cpu->memory, 0, MEM_SIZE);
    memset(cpu->V, 0, NUM_REGISTERS);
    memset(cpu->stack, 0, STACK_SIZE);
    cpu->I = 0;
    cpu->PC = -1;

    return cpu;
}

void ch8_quit(ch8_cpu *cpu)
{
    free(cpu);
}

void ch8_load_rom(ch8_cpu *cpu, uint8_t *program, size_t size)
{
    assert(size <= MAX_PROGRAM_SIZE);

    uint8_t *programStart = cpu->memory + PROGRAM_START_OFFSET;
    memcpy(programStart, program, size);
}
