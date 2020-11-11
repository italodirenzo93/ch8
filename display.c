#include "display.h"
#include "SDL.h"

#include <stdbool.h>

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
bool initialized = false;

void display_init()
{
    if (!initialized)
    {
        SDL_InitSubSystem(SDL_INIT_VIDEO);
        SDL_CreateWindowAndRenderer(800, 600, 0, &window, &renderer);
        SDL_SetWindowTitle(window, "CHIP-8");
        initialized = true;
    }
}

void display_quit()
{
    if (initialized)
    {
        SDL_DestroyRenderer(renderer);
        renderer = NULL;

        SDL_DestroyWindow(window);
        window = NULL;

        SDL_QuitSubSystem(SDL_INIT_VIDEO);

        initialized = false;
    }
}

void display_clear()
{
    SDL_RenderClear(renderer);
}
