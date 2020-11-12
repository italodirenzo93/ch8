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

    printf("CH8 Libertad [%d %s]\n", argc, argv[0]);

    ch8_init(&cpu);
    display_init();

    const char rom[] = "test_opcode.ch8";
    if (!ch8_load_rom_file(cpu, rom))
    {
        exit(EXIT_FAILURE);
    }

    SDL_Event ev;
    while (1)
    {
        if (cpu->running)
        {
            ch8_exec_opcode(cpu);
        }

        while (SDL_PollEvent(&ev))
        {
            switch (ev.type)
            {
            case SDL_QUIT:
                exit(EXIT_SUCCESS);
                break;
            }
        }
    }
}
