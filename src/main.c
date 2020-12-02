#include <stdio.h>
#include <time.h>
#include <SDL.h>

#include "ch8_cpu.h"
#include "display.h"
#include "keyboard.h"
#include "log.h"
#include "util.h"

ch8_cpu *cpu = NULL;
static SDL_Event event;

//static const uint8_t program[] = { 0x00, 0xE0 };
static const uint8_t program[] = {
    0x62, 0x10, // store 10 in V[2]
    0xF2, 0x15, // set delay timer to V[2]
    0x62, 0x10, // store 10 in V[2]
    0x62, 0x10, // store 10 in V[2]
    0x62, 0x10, // store 10 in V[2]
    0x62, 0x10, // store 10 in V[2]
    0x62, 0x10, // store 10 in V[2]
    0x62, 0x10, // store 10 in V[2]
    0x62, 0x10, // store 10 in V[2]
    0x62, 0x10, // store 10 in V[2]
    0x62, 0x10, // store 10 in V[2]
    0x62, 0x10, // store 10 in V[2]
};

static void initialize(int argc, char* argv[])
{
    // Allocate VM
    cpu = ch8_malloc(sizeof(ch8_cpu));

    // Initialize VM
    ch8_reset(cpu);
    srand((unsigned int)time(NULL));

    // Initialize sub-systems
    if (log_init() != 0) {
        printf("Could not initialize the logger\n");
        exit(EXIT_FAILURE);
    }

    if (display_init() != 0) {
        log_critical("Failed to initialize the display\n");
        exit(EXIT_FAILURE);
    }

    // Load test ROM
    //ch8_load_rom(cpu, program, SDL_arraysize(program));
    // TODO: Get ROM filename from argv
    if (!ch8_load_rom_file(cpu, "c8_test.c8")) {
        log_critical("Could not load ROM");
        exit(EXIT_FAILURE);
    }

    cpu->running = true;
}

static void cleanup(void)
{
    display_quit();
    ch8_free((void**)&cpu);
    printf("Freed CHIP-8 VM.\n");
}

static void window_message_loop(void)
{
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            exit(EXIT_SUCCESS);
            break;
        case SDL_KEYDOWN: {
            const input_key key = sdl_keycode_to_key_register(event.key.keysym.sym);
            set_key_down(cpu, key);
            break;
        }
        case SDL_KEYUP: {
            const input_key key = sdl_keycode_to_key_register(event.key.keysym.sym);
            set_key_up(cpu, key);
            break;
        }
        }
    }
}

int main(int argc, char *argv[])
{
    atexit(cleanup);
    initialize(argc, argv);

    float elapsed_ms = 0.0f;
    while (1) {
        window_message_loop();

        const uint64_t start = SDL_GetPerformanceCounter();

        if (cpu->running) {
            display_clear();
            cpu->running = ch8_clock_cycle(cpu, elapsed_ms);
            display_present(cpu);
        }

        const uint64_t end = SDL_GetPerformanceCounter();
        const float elapsed = (float)((end - start) * 1000) / SDL_GetPerformanceFrequency();

        // Cap the framerate to 60hz
        SDL_Delay((Uint32)SDL_floorf(16.666f - elapsed));

        elapsed_ms = elapsed;
    }
}
