#include <stdio.h>

#include "ch8_cpu.h"
#include "display.h"

ch8_cpu *cpu = NULL;

void cleanup()
{
    display_quit();
    if (cpu != NULL)
    {
        ch8_quit(&cpu);
        printf("Freed CHIP-8 VM.\n");
    }
}

int main(int argc, char *argv[])
{
    atexit(cleanup);

    ch8_init(&cpu);
    display_init();

    // if (!ch8_load_rom_file(cpu, "test_opcode.ch8"))
    // {
    //     exit(EXIT_FAILURE);
    // }

    while (1)
    {
        display_event_loop(cpu);

        if (cpu->running)
        {
            ch8_exec_opcode(cpu);
        }
    }
}
