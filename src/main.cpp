#include <stdio.h>
#include <time.h>
#include <SDL.h>
#include <imgui_impl_sdl2.h>

#include "ch8_cpu.h"
#include "ch8_display.h"
#include "ch8_audio.h"
#include "ch8_keyboard.h"
#include "ch8_log.h"
#include "ch8_util.h"

ch8_cpu cpu;

SDL_Window* window = NULL;

static void initialize(int argc, char* argv[])
{
    // Initialize VM
    ch8_reset(&cpu);
    srand((u32)time(NULL));

    // Initialize sub-systems
    if (ch8_logInit() != 0) {
        fprintf(stderr, "Could not initialize the logger\n");
        exit(EXIT_FAILURE);
    }

    // Load test ROM
    // TODO: Get ROM filename from argv
    if (!ch8_loadRomFile(&cpu, "assets/test_opcode.ch8")) {
        ch8_logCritical("Could not load ROM");
        exit(EXIT_FAILURE);
    }

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

    if (ch8_displayInit(static_cast<void*>(window)) != 0) {
        ch8_logCritical("Failed to initialize the display\n");
        exit(EXIT_FAILURE);
    }

    if (ch8_audioInit() != 0) {
        ch8_logCritical("Failed to initialize audio");
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
        ImGui_ImplSDL2_ProcessEvent(&event);

        switch (event.type) {
        case SDL_QUIT:
            exit(EXIT_SUCCESS);
            break;
        case SDL_KEYDOWN: {
            ch8_key key = __SDLKeycodeToKeyRegister(event.key.keysym.sym);
            if (key != KEY_UNKNOWN) {
                ch8_setKeyDown(&cpu, key);
                if (cpu.waitFlag) {
                    cpu.V[cpu.waitReg] = key;
                    cpu.waitFlag = false;
                }
            }
            break;
        }
        case SDL_KEYUP: {
            ch8_key key = __SDLKeycodeToKeyRegister(event.key.keysym.sym);
            if (key != KEY_UNKNOWN) {
                ch8_setKeyUp(&cpu, key);
            }
            break;
        }
        }
    }
}

int main(int argc, char *argv[])
{
    atexit(cleanup);
    initialize(argc, argv);

    f32 elapsedMs = 0.0f;

    while (1) {
        windowMessageLoop();

        u64 start, end;

        start = SDL_GetPerformanceCounter();

        if (!cpu.waitFlag && ch8_clockCycle(&cpu, elapsedMs)) {
            ch8_displayBeginFrame();
            if (cpu.drawFlag) {
                ch8_displayWriteFb(&cpu);
            }
            ch8_displayEndFrame();
        }

        end = SDL_GetPerformanceCounter();

        f32 elapsed = (f32)((end - start) * 1000) / SDL_GetPerformanceFrequency();

        // Cap the framerate to 60hz
        SDL_Delay((u32)SDL_floorf(16.666f - elapsed));

        // Update timers
        cpu.delayTimer = ch8_max(cpu.delayTimer - 1, 0);
        cpu.soundTimer = ch8_max(cpu.soundTimer - 1, 0);

        elapsedMs = elapsed;
    }
}
