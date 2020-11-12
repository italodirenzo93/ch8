#include "display.h"
#include "SDL.h"

#include <stdbool.h>

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *display = NULL;
SDL_Rect rect = {0, 0, 64, 32};
bool initialized = false;
int pitch = 64;

void display_init()
{
    if (!initialized)
    {
        SDL_InitSubSystem(SDL_INIT_VIDEO);
        SDL_CreateWindowAndRenderer(800, 600, 0, &window, &renderer);
        SDL_SetWindowTitle(window, "CHIP-8");

        window = SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, 0);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

        display = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, 64, 32);

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

void begin_frame(uint8_t *pixels)
{
    SDL_LockTexture(display, &rect, (void **)pixels, &pitch);
}

void end_frame()
{
    SDL_UnlockTexture(display);
}

void draw_display()
{
    SDL_RenderCopy(renderer, display, &rect, NULL);
    SDL_RenderPresent(renderer);
}
