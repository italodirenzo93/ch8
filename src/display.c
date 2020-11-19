#include <SDL.h>

#include "display.h"
#include "ch8_cpu.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *display = NULL;

SDL_Event event;
bool initialized = false;

#define INIT_CHECK()  \
    if (!initialized) \
    return

static input_key scancode_to_key_register(SDL_Scancode scancode)
{
    switch (scancode)
    {
    default:
        return INPUT_KEY_UNKNOWN;
    case SDL_SCANCODE_UP:
        return INPUT_KEY_UP;
    case SDL_SCANCODE_DOWN:
        return INPUT_KEY_DOWN;
    case SDL_SCANCODE_LEFT:
        return INPUT_KEY_LEFT;
    case SDL_SCANCODE_RIGHT:
        return INPUT_KEY_RIGHT;
    }
}

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

void display_event_loop(ch8_cpu *cpu)
{
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            exit(EXIT_SUCCESS);
            break;
        case SDL_KEYDOWN:
        {
            //printf("Key down: %s\n", SDL_GetScancodeName(event.key.keysym.scancode));
            input_key key = scancode_to_key_register(event.key.keysym.scancode);
            if (key != INPUT_KEY_UNKNOWN)
            {
                cpu->keypad[key] = CH8_KEYDOWN;
            }
            break;
        }
        case SDL_KEYUP:
        {
            //printf("Key up: %s\n", SDL_GetScancodeName(event.key.keysym.scancode));
            input_key key = scancode_to_key_register(event.key.keysym.scancode);
            if (key != INPUT_KEY_UNKNOWN)
            {
                cpu->keypad[key] = CH8_KEYUP;
            }
            break;
        }
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

    SDL_Rect src = { 0 };
    src.x = x;
    src.y = y;
    src.w = 8;
    src.h = h + 1;

    SDL_Surface *surface = NULL;
    SDL_LockTextureToSurface(display, &src, &surface);
    SDL_memcpy(surface->pixels, &cpu->memory[cpu->I], (size_t)src.w * src.h);
    SDL_UnlockTexture(display);

    SDL_Rect dest = { 0 };
    dest.x = src.x = 0;
    dest.y = src.y = 0;

    src.w = DISPLAY_WIDTH;
    src.h = DISPLAY_HEIGHT;

    dest.w = WINDOW_WIDTH;
    dest.h = WINDOW_HEIGHT;

    SDL_RenderCopy(renderer, display, &src, &dest);

    SDL_RenderPresent(renderer);
}
