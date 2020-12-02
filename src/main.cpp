#include <iostream>

#include <SDL.h>

#include "log.hpp"
#include "cpu.hpp"
#include "Exception.hpp"

SDL_Window* window = nullptr;
SDL_Event ev = { 0 };
static bool running = true;

static void Initialize()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        std::string msg = "SDL failed to initialize: ";
        msg += SDL_GetError();
        throw ch8::Exception(msg);
    }

    window = SDL_CreateWindow(
        "CHIP-8",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        640, 320,
        0
    );

    if (window == nullptr) {
        std::string msg = "Failed to create window: ";
        msg += SDL_GetError();
        throw ch8::Exception(msg);
    }

    ch8::log::init();
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
    std::cout << "Hello World!\n";

    try {
        Initialize();

        ch8::Cpu cpu;
        cpu.LoadRomFromFile("test_opcode.ch8");

        while (running) {
            WindowMessageLoop();
        }

        Cleanup();
    }
    catch (const ch8::Exception& e) {
        std::cerr << "CHIP-8 error : " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (...) {
        std::cerr << "Unknown fatal error..." << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
