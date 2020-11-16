#include <stdio.h>

#include "ch8_cpu.h"
#include "display.h"
#include "SDL.h"

ch8_cpu *cpu = NULL;

void cleanup()
{
    if (cpu != NULL)
    {
        ch8_quit(&cpu);
        printf("Freed CHIP-8 VM.\n");
    }
    display_quit();
}

int main(int argc, char *argv[])
{
    atexit(cleanup);

    ch8_init(&cpu);
    display_init();

    if (!ch8_load_rom_file(cpu, "test_opcode.ch8"))
    {
        exit(EXIT_FAILURE);
    }

    SDL_Event ev;
    while (1)
    {
        display_poll_events();

        if (cpu->running)
        {
            ch8_exec_opcode(cpu);
        }
    }
}
