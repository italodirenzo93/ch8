#include "keyboard.h"

#include <assert.h>

#include <SDL_events.h>
#include <SDL_keyboard.h>

#include "ch8_cpu.h"
#include "log.h"

input_key sdl_scancode_to_key_register(SDL_Scancode scancode)
{
    switch (scancode)
    {
    case SDL_SCANCODE_0:
    case SDL_SCANCODE_KP_0:
        return KEY_0;

    case SDL_SCANCODE_1:
    case SDL_SCANCODE_KP_1:
        return KEY_1;

    case SDL_SCANCODE_2:
    case SDL_SCANCODE_KP_2:
        return KEY_2;

    case SDL_SCANCODE_3:
    case SDL_SCANCODE_KP_3:
        return KEY_3;

    case SDL_SCANCODE_4:
    case SDL_SCANCODE_KP_4:
        return KEY_4;

    case SDL_SCANCODE_5:
    case SDL_SCANCODE_KP_5:
        return KEY_5;

    case SDL_SCANCODE_6:
    case SDL_SCANCODE_KP_6:
        return KEY_6;

    case SDL_SCANCODE_7:
    case SDL_SCANCODE_KP_7:
        return KEY_7;

    case SDL_SCANCODE_8:
    case SDL_SCANCODE_KP_8:
        return KEY_8;

    case SDL_SCANCODE_9:
    case SDL_SCANCODE_KP_9:
        return KEY_9;

    case SDL_SCANCODE_KP_A:
        return KEY_A;

    case SDL_SCANCODE_KP_B:
        return KEY_B;

    case SDL_SCANCODE_KP_C:
        return KEY_C;

    case SDL_SCANCODE_KP_D:
        return KEY_D;

    case SDL_SCANCODE_KP_E:
        return KEY_E;

    case SDL_SCANCODE_KP_F:
        return KEY_F;
        
    default:
        return KEY_UNKNOWN;
    }
}

bool is_key_down(const ch8_cpu *cpu, input_key key)
{
    assert(cpu != NULL);
    if (key == KEY_UNKNOWN) {
        return false;
    }
    return cpu->keypad[key] == CH8_KEYSTATE_DOWN;
}

bool is_key_up(const ch8_cpu *cpu, input_key key)
{
    assert(cpu != NULL);
    if (key == KEY_UNKNOWN) {
        return false;
    }
    return cpu->keypad[key] == CH8_KEYSTATE_UP;
}

void set_key_down(ch8_cpu *cpu, input_key key)
{
    assert(cpu != NULL);
    if (key != KEY_UNKNOWN) {
        cpu->keypad[key] = CH8_KEYSTATE_DOWN;
    }
}

void set_key_up(ch8_cpu *cpu, input_key key)
{
    assert(cpu != NULL);
    if (key != KEY_UNKNOWN) {
        cpu->keypad[key] = CH8_KEYSTATE_UP;
    }
}

int await_keypress(ch8_cpu *cpu, input_key *key)
{
    assert(cpu != NULL);
    
    SDL_Event event = { 0 };
    while (event.type != SDL_KEYDOWN) {
        if (SDL_WaitEvent(&event) != 1) {
            log_debug("Error awaiting keypress: %s", SDL_GetError());
            return 1;
        }
    }
    
    (*key) = sdl_scancode_to_key_register(event.key.keysym.scancode);
    if ((*key) != KEY_UNKNOWN) {
        set_key_down(cpu, (*key));
    }
    
    return 0;
}
