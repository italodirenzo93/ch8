#include <stdio.h>
#include "ch8_cpu.h"

int main(int argc, char *argv[])
{
    printf("CH8 Libertad [%d %s]\n", argc, argv[0]);

    uint8_t program[] = {0x00, 0xEE};

    ch8_gpu *gpu = NULL;
    ch8_init_gpu(&gpu);

    ch8_cpu *cpu = NULL;
    ch8_init(gpu, &cpu);

    ch8_load_rom(cpu, program, 2);
    uint16_t opcode = ch8_next_opcode(cpu);
    printf("Opcode is %d\n", opcode);
    ch8_exec_opcode(cpu, opcode);

    ch8_quit(cpu);
    cpu = NULL;

    return 0;
}
