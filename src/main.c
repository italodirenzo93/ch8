#include <stdio.h>
#include <SDL.h>

#include "ch8_cpu.h"
#include "display.h"
#include "log.h"
#include "util.h"

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
    ch8_free((void**)&cpu);
    printf("Freed CHIP-8 VM.\n");
}

int main(int argc, char *argv[])
{
    atexit(cleanup);

    // Allocate VM
    cpu = ch8_malloc(sizeof(ch8_cpu));

    // Initialize VM
    ch8_reset(cpu);

    // Initialize sub-systems
    if (log_init() != 0) {
        printf("Could not initialize the logger\n");
        exit(EXIT_FAILURE);
    }

    if (display_init() != 0) {
        log_critical("Failed to initialize the display\n");
        exit(EXIT_FAILURE);
    }

    //ch8_load_rom(cpu, program, SDL_arraysize(program));

    // Load test ROM
    if (!ch8_load_rom_file(cpu, "test_opcode.ch8")) {
        log_critical("Could not load ROM");
        exit(EXIT_FAILURE);
    }

    cpu->running = true;
    while (1) {
        display_event_loop(cpu);

        const uint64_t start = SDL_GetPerformanceCounter();

        if (cpu->running) {
            display_clear();
            cpu->running = ch8_clock_cycle(cpu);
            display_present(cpu);
        }

        const uint64_t end = SDL_GetPerformanceCounter();
        const float elapsed_ms = (float) (end - start) / (float) SDL_GetPerformanceFrequency() * 1000;

        if (cpu->running){
            ch8_update_timers(cpu, elapsed_ms);
        }

        // Cap the framerate to 60hz
        SDL_Delay((Uint32) floorf(16.666f - elapsed_ms));
    }
}
