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
static SDL_Window* window = NULL;

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
    // Initialize SDL and create window
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    window = SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 320, 0);
    if (window == NULL) {
        fprintf(stderr, "Could not create SDL window: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

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

    if (ch8_displayInit((void*)window) != 0) {
        log_critical("Failed to initialize the display\n");
        exit(EXIT_FAILURE);
    }

    // Load test ROM
    //ch8_load_rom(cpu, program, SDL_arraysize(program));
    // TODO: Get ROM filename from argv
    if (!ch8_loadRomFile(cpu, "test_opcode.c8")) {
        log_critical("Could not load ROM");
        exit(EXIT_FAILURE);
    }

    cpu->running = true;
}

static void cleanup(void)
{
    ch8_displayQuit();
    ch8_free((void**)&cpu);
    printf("Freed CHIP-8 VM.\n");
}

static void windowMessageLoop(void)
{
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            exit(EXIT_SUCCESS);
            break;
        case SDL_KEYDOWN: {
            const ch8_key key = sdl_keycode_to_key_register(event.key.keysym.sym);
            ch8_setKeyDown(cpu, key);
            break;
        }
        case SDL_KEYUP: {
            const ch8_key key = sdl_keycode_to_key_register(event.key.keysym.sym);
            ch8_setKeyUp(cpu, key);
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
        windowMessageLoop();

        const uint64_t start = SDL_GetPerformanceCounter();

        if (cpu->running) {
            ch8_displayClear();
            cpu->running = ch8_clockCycle(cpu, elapsed_ms);
            ch8_displayPresent(cpu);
        }

        const uint64_t end = SDL_GetPerformanceCounter();
        const float elapsed = (float)((end - start) * 1000) / SDL_GetPerformanceFrequency();

        // Cap the framerate to 60hz
        SDL_Delay((Uint32)SDL_floorf(16.666f - elapsed));

        elapsed_ms = elapsed;
    }
}
