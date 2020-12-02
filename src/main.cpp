#include <iostream>

#include <SDL.h>

SDL_Window* window = nullptr;
SDL_Event ev = { 0 };
static bool running = true;

static void initialize()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        std::string msg = "SDL failed to initialize: ";
        msg += SDL_GetError();
        throw std::exception(msg.c_str());
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
        throw std::exception(msg.c_str());
    }
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

        while (running) {
            window_message_loop();
        }

        cleanup();
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
