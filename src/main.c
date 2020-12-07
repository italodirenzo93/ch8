#include <stdio.h>
#include <time.h>
#include <SDL.h>

#include "ch8_cpu.h"
#include "ch8_display.h"
#include "ch8_audio.h"
#include "ch8_keyboard.h"
#include "ch8_log.h"
#include "ch8_util.h"

ch8_cpu cpu = { 0 };

SDL_Window* window = NULL;

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

    // Initialize VM
    ch8_reset(&cpu);
    srand((unsigned int)time(NULL));

    // Initialize sub-systems
    if (ch8_logInit() != 0) {
        printf("Could not initialize the logger\n");
        exit(EXIT_FAILURE);
    }

    if (ch8_displayInit((void*)window) != 0) {
        ch8_logCritical("Failed to initialize the display\n");
        exit(EXIT_FAILURE);
    }

    if (ch8_audioInit() != 0) {
        ch8_logCritical("Failed to initialize audio");
    }

    // Load test ROM
    //ch8_load_rom(cpu, program, SDL_arraysize(program));
    // TODO: Get ROM filename from argv
    if (!ch8_loadRomFile(&cpu, "test_opcode.ch8")) {
        ch8_logCritical("Could not load ROM");
        exit(EXIT_FAILURE);
    }
}

static void cleanup(void)
{
    ch8_displayQuit();
    ch8_audioQuit();
    ch8_logQuit();

    if (window != NULL) {
        SDL_DestroyWindow(window);
        window = NULL;
    }

    SDL_Quit();

    printf("Freed CHIP-8 VM.\n");
}

static void windowMessageLoop(void)
{
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            exit(EXIT_SUCCESS);
            break;
        }
    }
}

int main(int argc, char *argv[])
{
    atexit(cleanup);
    initialize(argc, argv);

    float elapsedMs = 0.0f;
    while (1) {
        windowMessageLoop();

        uint64_t start, end;

        start = SDL_GetPerformanceCounter();

        if (ch8_clockCycle(&cpu, elapsedMs)) {
            ch8_pollKeyboardInput(&cpu);
            if (cpu.drawFlag) {
                ch8_displayWriteFb(&cpu);
            }
            ch8_displayUpdate();
        }

        end = SDL_GetPerformanceCounter();

        float elapsed = (float)((end - start) * 1000) / SDL_GetPerformanceFrequency();

        // Cap the framerate to 60hz
        //SDL_Delay((Uint32)SDL_floorf(16.666f - elapsed));

        elapsedMs = elapsed;
    }
}
