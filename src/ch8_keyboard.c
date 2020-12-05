#include "ch8_keyboard.h"

#include <assert.h>

#include <SDL_events.h>

#include "ch8_cpu.h"
#include "ch8_log.h"

ch8_key sdl_keycode_to_key_register(SDL_Keycode keycode)
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
    
    (*key) = sdl_keycode_to_key_register(event.key.keysym.sym);
    if ((*key) != KEY_UNKNOWN) {
        ch8_setKeyDown(cpu, (*key));
    }
    
    return 0;
}
