#include <iostream>

#include <SDL.h>

#include "log.hpp"
#include "cpu.hpp"

SDL_Window* window = nullptr;
SDL_Event ev = { 0 };
static bool running = true;

static void initialize()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        std::string msg = "SDL failed to initialize: ";
        msg += SDL_GetError();
        throw ch8::exception(msg);
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
        throw ch8::exception(msg);
    }

    ch8::log::init();
}

static void cleanup()
{
    if (window != nullptr) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    SDL_Quit();
}

static void window_message_loop()
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
        initialize();

        ch8::cpu cpu;
        cpu.load_rom("test_opcode.ch8");

        while (running) {
            window_message_loop();
        }

        cleanup();
    }
    catch (const ch8::exception& e) {
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
