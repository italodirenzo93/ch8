#include <stdio.h>
#include "ch8_cpu.h"

#include "SDL.h"

uint8_t program[] = {0x00, 0xEE};
ch8_gpu *gpu = NULL;
ch8_cpu *cpu = NULL;

void cleanup()
{
    if (gpu != NULL)
    {
        ch8_quit_gpu(&gpu);
        printf("Free'd GPU\n");
    }

    if (cpu != NULL)
    {
        ch8_quit(&cpu);
        printf("Free'd CPU\n");
    }

    SDL_Quit();
}

int main(int argc, char *argv[])
{
    atexit(cleanup);

    printf("CH8 Libertad [%d %s]\n", argc, argv[0]);

    ch8_init_gpu(&gpu);
    ch8_init(gpu, &cpu);

    ch8_load_rom(cpu, program, 2);

    uint16_t opcode;
    SDL_Event ev;
    while ((opcode = ch8_next_opcode(cpu)))
    {
        ch8_exec_opcode(cpu, opcode);
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

    return EXIT_SUCCESS;
}
