#include <cstdio>

#include <SDL.h>

#include "Log.hpp"
#include "Cpu.hpp"

SDL_Window* window = nullptr;
SDL_Event ev = { 0 };
static bool running = true;

static bool Initialize()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        ch8::log::critical("SDL failed to initialize : %s", SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow(
        "CHIP-8",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        640, 320,
        0
    );

    if (window == nullptr) {
        ch8::log::critical("Failed to create SDL Window : %s", SDL_GetError());
        return false;
    }

    ch8::log::init();

    return true;
}

static void Cleanup()
{
    if (window != nullptr) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    SDL_Quit();
}

static void WindowMessageLoop()
{
    while (SDL_PollEvent(&ev)) {
        switch (ev.type) {
        case SDL_QUIT:
            running = false;
            break;
        }
    }
}

int main(int argc, char* argv[])
{
    atexit(Cleanup);

    if (!Initialize()) {
        fprintf(stderr, "Unable to initialize the application\n");
        return EXIT_FAILURE;
    }

    ch8::Cpu cpu;

    if (!cpu.LoadRomFromFile("test_opcode.ch8")) {
        ch8::log::critical("Could not load ROM %s...", "test_opcode.ch8");
        return EXIT_FAILURE;
    }

    cpu.Start();
    while (running) {
        WindowMessageLoop();

        if (cpu.IsRunning()) {
            ch8::Cpu::opcode_t opcode = cpu.GetNextOpcode();

            switch (opcode) {
            case 0x00EE:
                cpu.PopAddress();
                break;
            case 0x001E:
                cpu.JumpTo(0xDE);
                break;
            default:
                ch8::log::error("Unrecognized opcode %X", opcode);
                cpu.Stop();
                break;
            }
        }
    }

    return EXIT_SUCCESS;
}
