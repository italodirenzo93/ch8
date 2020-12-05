#include "display.h"

#include <assert.h>
#include <SDL.h>

#include "ch8_cpu.h"
#include "keyboard.h"
#include "log.h"

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Texture *display = NULL;
static SDL_PixelFormat *pixelFormat = NULL;

static SDL_Event event;
static bool initialized = false;

#define INIT_CHECK() if (!initialized) return

int ch8_displayInit()
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

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
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

    pixelFormat = SDL_AllocFormat(SDL_PIXELFORMAT_RGB888);
    if (pixelFormat == NULL) {
        log_error("Failed to allocate pixel format: %s", SDL_GetError());
        return 1;
    }

    initialized = true;

    return 0;
}

void ch8_displayQuit()
{
    INIT_CHECK();

    SDL_FreeFormat(pixelFormat);
    pixelFormat = NULL;
    log_debug("Pixel format freed");

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

void ch8_displayClear()
{
    INIT_CHECK();
    SDL_RenderClear(renderer);
}

void ch8_displayPresent(const ch8_cpu* cpu)
{
    INIT_CHECK();
    assert(cpu != NULL);
    SDL_RenderCopy(renderer, display, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void ch8_displayWriteFb(const ch8_cpu* cpu)
{
    INIT_CHECK();
    assert(cpu != NULL);

    uint8_t* pixels = NULL;
    int x, y, pitch;

    SDL_LockTexture(display, NULL, (void**)&pixels, &pitch);

    for (y = 0; y < CH8_DISPLAY_HEIGHT; y++) {
        uint32_t* p = (uint32_t*)(pixels + pitch * y);
        for (x = 0; x < CH8_DISPLAY_WIDTH; x++) {
            const uint16_t i = y * CH8_DISPLAY_WIDTH + x;
            const uint8_t color = ch8_getPixel(cpu, x, y) > 0 ? 255 : 0;
            *p = SDL_MapRGB(pixelFormat, color, color, color);
            p++;
        }
    }

    SDL_UnlockTexture(display);
}
