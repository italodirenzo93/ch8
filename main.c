#include <stdio.h>

#include "ch8_cpu.h"

static const uint8_t program[] = {0x000, 0x000, 0x000, 0x000};

int main(int argc, char *argv[])
{
    printf("hello world [%d %s]\n", argc, argv[0]);

    ch8_cpu *cpu = ch8_init();

    ch8_load_rom(cpu, program, 4);

    ch8_quit(cpu);
    cpu = NULL;

    return 0;
}
