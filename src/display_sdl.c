#include "display.h"
#include "ch8_cpu.h"
#include "SDL.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *display = NULL;

SDL_Event event;
bool initialized = false;

#define INIT_CHECK()  \
    if (!initialized) \
    return

void display_init()
{
    if (initialized)
        return;

    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    display = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, DISPLAY_WIDTH, DISPLAY_HEIGHT);

    initialized = true;
}

void display_quit()
{
    INIT_CHECK();

    SDL_DestroyTexture(display);
    display = NULL;

    SDL_DestroyRenderer(renderer);
    renderer = NULL;

    SDL_DestroyWindow(window);
    window = NULL;

    SDL_Quit();

    initialized = false;
}

void display_event_loop()
{
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            exit(EXIT_SUCCESS);
            break;
        }
    }
}

void display_clear()
{
    INIT_CHECK();
    SDL_RenderClear(renderer);
}

void draw_sprite(ch8_cpu *cpu, uint8_t x, uint8_t y, uint8_t h)
{
    INIT_CHECK();

    SDL_Rect src;
    src.x = x;
    src.y = y;
    src.w = 8;
    src.h = h + 1;

    SDL_Surface *surface = NULL;
    SDL_LockTextureToSurface(display, &src, &surface);
    SDL_memcpy(surface->pixels, &cpu->memory[cpu->I], src.w * src.h);
    SDL_UnlockTexture(display);

    SDL_Rect dest;
    dest.x = 0;
    dest.y = 0;
    dest.w = WINDOW_WIDTH;
    dest.h = WINDOW_HEIGHT;
    SDL_RenderCopy(renderer, display, &src, &dest);

    SDL_RenderPresent(renderer);
}
