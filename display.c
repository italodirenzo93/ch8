#include "display.h"
#include "SDL.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

void display_init()
{
    SDL_InitSubSystem(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(800, 600, 0, &window, &renderer);
}

void display_quit()
{
    SDL_DestroyRenderer(renderer);
    renderer = NULL;

    SDL_DestroyWindow(window);
    window = NULL;

    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}
