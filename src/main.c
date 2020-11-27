#include <stdio.h>
#include <SDL.h>

#include "ch8_cpu.h"
#include "display.h"
#include "log.h"

ch8_cpu *cpu = NULL;

static const uint8_t program[] = { 0x00, 0xE0 };
// clang-format off
static const uint8_t fontROM[] = {
  // 4x5 font sprites (0-F)
  0xF0, 0x90, 0x90, 0x90, 0xF0,
  0x20, 0x60, 0x20, 0x20, 0x70,
  0xF0, 0x10, 0xF0, 0x80, 0xF0,
  0xF0, 0x10, 0xF0, 0x10, 0xF0,
  0xA0, 0xA0, 0xF0, 0x20, 0x20,
  0xF0, 0x80, 0xF0, 0x10, 0xF0,
  0xF0, 0x80, 0xF0, 0x90, 0xF0,
  0xF0, 0x10, 0x20, 0x40, 0x40,
  0xF0, 0x90, 0xF0, 0x90, 0xF0,
  0xF0, 0x90, 0xF0, 0x10, 0xF0,
  0xF0, 0x90, 0xF0, 0x90, 0x90,
  0xE0, 0x90, 0xE0, 0x90, 0xE0,
  0xF0, 0x80, 0x80, 0x80, 0xF0,
  0xE0, 0x90, 0x90, 0x90, 0xE0,
  0xF0, 0x80, 0xF0, 0x80, 0xF0,
  0xF0, 0x80, 0xF0, 0x80, 0x80,
};

static void cleanup(void)
{
    display_quit();
    if (cpu != NULL) {
        ch8_quit(&cpu);
        printf("Freed CHIP-8 VM.\n");
    }
}

int main(int argc, char *argv[])
{
    atexit(cleanup);
    
    if (log_init() != 0) {
        printf("Could not initialize the logger\n");
        exit(EXIT_FAILURE);
    }

    if (ch8_init(&cpu) != 0) {
        log_error("Failed to initialize CHIP-8 VM\n");
        exit(EXIT_FAILURE);
    }
    
    if (display_init() != 0) {
        log_error("Failed to initialize the display\n");
        exit(EXIT_FAILURE);
    }

    //ch8_load_rom(cpu, program, SDL_arraysize(program));

    if (!ch8_load_rom_file(cpu, "test_opcode.ch8")) {
        log_critical("Could not load ROM");
        exit(EXIT_FAILURE);
    }

    while (1) {
        display_event_loop(cpu);
        
        const uint64_t start = SDL_GetPerformanceCounter();
        
        if (cpu->running) {
            cpu->running = ch8_exec_opcode(cpu);
        }
        
        const uint64_t end = SDL_GetPerformanceCounter();
        const float elapsed_ms = (float) (end - start) / (float) SDL_GetPerformanceFrequency() * 1000;
        
        SDL_Delay(floorf(16.666f - elapsed_ms));

        cpu->delay_timer = SDL_max(0, cpu->delay_timer - 1);
        cpu->sound_timer = SDL_max(0, cpu->sound_timer - 1);
    }
}
