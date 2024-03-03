#include "ch8_display.h"

#include <assert.h>
#include <SDL.h>

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>

#include "ch8_cpu.h"
#include "ch8_log.h"

static SDL_Renderer *renderer = nullptr;
static SDL_Texture *display = nullptr;
static SDL_PixelFormat *pixelFormat = nullptr;
static ImGuiContext *imgui = nullptr;

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

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    imgui = ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    if (!ImGui_ImplSDL2_InitForSDLRenderer(static_cast<SDL_Window*>(handle), renderer)) {
        ch8_logError("Failed to initialize ImGui SDL2 window backend.");
        return 1;
    }
    if (!ImGui_ImplSDLRenderer2_Init(renderer)) {
        ch8_logError("Failed to initialize ImGui SDL2 renderer backend.");
        return 1;
    }

    initialized = true;

    return 0;
}

void ch8_displayQuit()
{
    INIT_CHECK();

    // ImGui cleanup
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    imgui = nullptr;
    ch8_logDebug("ImGui resources freed");

    // Cleanup display buffer resources
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

void ch8_displayBeginFrame()
{
    INIT_CHECK();

    // Clear display
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    // Start the Dear ImGui frame
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

void ch8_displayEndFrame()
{
    INIT_CHECK();

    // Render display buffer
    SDL_RenderCopy(renderer, display, NULL, NULL);

    // Render UI
    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());

    // Boom goes the dynamite
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
            u8 color = ch8_getPixel(cpu, x, y) > 0 ? 255 : 0;
            *p = SDL_MapRGB(pixelFormat, color, color, color);
            p++;
        }
    }

    SDL_UnlockTexture(display);
}
