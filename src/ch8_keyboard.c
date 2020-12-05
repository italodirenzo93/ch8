#include "ch8_keyboard.h"

#include <assert.h>

#include <SDL.h>

#include "ch8_cpu.h"
#include "ch8_log.h"

ch8_key __SDLKeycodeToKeyRegister(SDL_Keycode keycode)
{
    switch (keycode)
    {
    case SDLK_KP_0:
        return KEY_0;
    case SDLK_KP_1:
        return KEY_1;
    case SDLK_KP_2:
        return KEY_2;
    case SDLK_KP_3:
        return KEY_3;
    case SDLK_KP_4:
        return KEY_4;
    case SDLK_KP_5:
        return KEY_5;
    case SDLK_KP_6:
        return KEY_6;
    case SDLK_KP_7:
        return KEY_7;
    case SDLK_KP_8:
        return KEY_8;
    case SDLK_KP_9:
        return KEY_9;
    case SDLK_KP_A:
        return KEY_A;
    case SDLK_KP_B:
        return KEY_B;
    case SDLK_KP_C:
        return KEY_C;
    case SDLK_KP_D:
        return KEY_D;
    case SDLK_KP_E:
        return KEY_E;
    case SDLK_KP_F:
        return KEY_F;
    default:
        return KEY_UNKNOWN;
    }
}

bool ch8_isKeyDown(const ch8_cpu *cpu, ch8_key key)
{
    assert(cpu != NULL);
    if (key == KEY_UNKNOWN) {
        return false;
    }
    return cpu->keypad[key] == true;
}

bool ch8_isKeyUp(const ch8_cpu *cpu, ch8_key key)
{
    assert(cpu != NULL);
    if (key == KEY_UNKNOWN) {
        return false;
    }
    return cpu->keypad[key] == false;
}

void ch8_setKeyDown(ch8_cpu *cpu, ch8_key key)
{
    assert(cpu != NULL);
    if (key != KEY_UNKNOWN) {
        cpu->keypad[key] = true;
    }
}

void ch8_setKeyUp(ch8_cpu *cpu, ch8_key key)
{
    assert(cpu != NULL);
    if (key != KEY_UNKNOWN) {
        cpu->keypad[key] = false;
    }
}

int ch8_awaitKeyPress(ch8_cpu *cpu, ch8_key *key)
{
    assert(cpu != NULL);
    
    SDL_Event event = { 0 };
    while (event.type != SDL_KEYDOWN) {
        if (SDL_WaitEvent(&event) != 1) {
            ch8_logDebug("Error awaiting keypress: %s", SDL_GetError());
            return 1;
        }
    }
    
    (*key) = __SDLKeycodeToKeyRegister(event.key.keysym.sym);
    if ((*key) != KEY_UNKNOWN) {
        ch8_setKeyDown(cpu, (*key));
    }
    
    return 0;
}

void ch8_pollKeyboardInput(ch8_cpu* cpu)
{
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    cpu->keypad[KEY_0] = keys[SDL_SCANCODE_X];
    cpu->keypad[KEY_1] = keys[SDL_SCANCODE_1];
    cpu->keypad[KEY_2] = keys[SDL_SCANCODE_2];
    cpu->keypad[KEY_3] = keys[SDL_SCANCODE_3];
    cpu->keypad[KEY_4] = keys[SDL_SCANCODE_Q];
    cpu->keypad[KEY_5] = keys[SDL_SCANCODE_W];
    cpu->keypad[KEY_6] = keys[SDL_SCANCODE_E];
    cpu->keypad[KEY_7] = keys[SDL_SCANCODE_A];
    cpu->keypad[KEY_8] = keys[SDL_SCANCODE_S];
    cpu->keypad[KEY_9] = keys[SDL_SCANCODE_D];
    cpu->keypad[KEY_A] = keys[SDL_SCANCODE_Z];
    cpu->keypad[KEY_B] = keys[SDL_SCANCODE_C];
    cpu->keypad[KEY_C] = keys[SDL_SCANCODE_4];
    cpu->keypad[KEY_D] = keys[SDL_SCANCODE_R];
    cpu->keypad[KEY_E] = keys[SDL_SCANCODE_F];
    cpu->keypad[KEY_F] = keys[SDL_SCANCODE_V];
}
