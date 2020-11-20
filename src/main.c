#include <stdio.h>
#include <SDL.h>

#include "ch8_cpu.h"
#include "display.h"
#include "log.h"

ch8_cpu *cpu = NULL;

void cleanup(void)
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
    
    if (log_init() != 0)
    {
        printf("Could not initialize the logger\n");
        exit(EXIT_FAILURE);
    }

    if (ch8_init(&cpu) != 0)
    {
        log_error("Failed to initialize CHIP-8 VM\n");
        exit(EXIT_FAILURE);
    }
    
    if (display_init() != 0)
    {
        log_error("Failed to initialize the display\n");
        exit(EXIT_FAILURE);
    }

    /*if (!ch8_load_rom_file(cpu, "test_opcode.ch8"))
    {
        exit(EXIT_FAILURE);
    }*/

    while (1)
    {
        display_event_loop(cpu);

        if (cpu->running)
        {
            ch8_exec_opcode(cpu);
        }
    }
}
