#include "ch8_display.h"

#include <assert.h>
#include <SDL.h>

#include "ch8_cpu.h"
#include "ch8_log.h"

static SDL_Renderer *renderer = NULL;
static SDL_Texture *display = NULL;
static SDL_PixelFormat *pixelFormat = NULL;

static bool initialized = false;

#define INIT_CHECK() if (!initialized) return

int ch8_displayInit(void* handle)
{
    if (initialized) {
        ch8_logInfo("Display sub-system already initialized\n");
        return 1; // designate some kind of error code
    }

    renderer = SDL_CreateRenderer((SDL_Window*)handle, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL)
    {
        ch8_logError("Failed to create renderer: %s\n", SDL_GetError());
        return 1;
    }

    display = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, CH8_DISPLAY_WIDTH, CH8_DISPLAY_HEIGHT);
    if (display == NULL)
    {
        ch8_logError("Failed to create render target: %s\n", SDL_GetError());
        return 1;
    }

    pixelFormat = SDL_AllocFormat(SDL_PIXELFORMAT_RGB888);
    if (pixelFormat == NULL) {
        ch8_logError("Failed to allocate pixel format: %s", SDL_GetError());
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
    ch8_logDebug("Pixel format freed");

    SDL_DestroyTexture(display);
    display = NULL;
    ch8_logDebug("Render target destroyed\n");

    SDL_DestroyRenderer(renderer);
    renderer = NULL;
    ch8_logDebug("Renderer destroyed\n");

    initialized = false;
}

void ch8_displayUpdate()
{
    INIT_CHECK();
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, display, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void ch8_displayWriteFb(const ch8_cpu* cpu)
{
    INIT_CHECK();
    assert(cpu != NULL);

    u8* pixels = NULL;
    int x, y, pitch;

    SDL_LockTexture(display, NULL, (void**)&pixels, &pitch);

    for (y = 0; y < CH8_DISPLAY_HEIGHT; y++) {
        u32* p = (u32*)(pixels + pitch * y);
        for (x = 0; x < CH8_DISPLAY_WIDTH; x++) {
            u16 i = y * CH8_DISPLAY_WIDTH + x;
            u8 color = ch8_getPixel(cpu, x, y) > 0 ? 255 : 0;
            *p = SDL_MapRGB(pixelFormat, color, color, color);
            p++;
        }
    }

    SDL_UnlockTexture(display);
}
