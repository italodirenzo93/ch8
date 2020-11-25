#include <SDL.h>

#include "display.h"
#include "ch8_cpu.h"
#include "log.h"
#include "input.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *display = NULL;

SDL_Event event;
bool initialized = false;

#define INIT_CHECK() if (!initialized) return

int display_init()
{
    if (initialized)
    {
        log_info("Display sub-system already initialized\n");
        return 1; // designate some kind of error code
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        log_error("Unable to initialize SDL: %s\n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (window == NULL)
    {
        log_error("Failed to create window: %s\n", SDL_GetError());
        return 1;
    }
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
    {
        log_error("Failed to create renderer: %s\n", SDL_GetError());
        return 1;
    }
    
    display = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, CH8_DISPLAY_WIDTH, CH8_DISPLAY_HEIGHT);
    if (display == NULL)
    {
        log_error("Failed to create render target: %s\n", SDL_GetError());
        return 1;
    }

    initialized = true;
    
    return 0;
}

void display_quit()
{
    INIT_CHECK();

    SDL_DestroyTexture(display);
    display = NULL;
    log_debug("Render target destroyed\n");

    SDL_DestroyRenderer(renderer);
    renderer = NULL;
    log_debug("Renderer destroyed\n");

    SDL_DestroyWindow(window);
    window = NULL;
    log_debug("Window destroyed\n");

    SDL_Quit();
    log_debug("Quit SDL\n");

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
            input_key key = _scancode_to_key_register(event.key.keysym.scancode);
            set_key_down(cpu, key);
            break;
        }
        case SDL_KEYUP:
        {
            input_key key = _scancode_to_key_register(event.key.keysym.scancode);
            set_key_up(cpu, key);
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

void display_present()
{
    INIT_CHECK();
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 1);
    SDL_RenderPresent(renderer);
}

void display_fb_copy(ch8_cpu *cpu)
{
    INIT_CHECK();
    
    SDL_Rect src = { 0 };
    src.x = 0;
    src.y = 0;
    src.w = CH8_DISPLAY_WIDTH;
    src.h = CH8_DISPLAY_HEIGHT;

    SDL_Surface *surface = NULL;
    SDL_LockTextureToSurface(display, &src, &surface);
    SDL_memcpy(surface->pixels, &cpu->memory[cpu->index_register], (size_t)src.w * src.h);
    SDL_UnlockTexture(display);

    SDL_Rect dest = { 0 };
    dest.x = 0;
    dest.y = 0;
    dest.w = WINDOW_WIDTH;
    dest.h = WINDOW_HEIGHT;

    SDL_RenderCopy(renderer, display, &src, &dest);
}
