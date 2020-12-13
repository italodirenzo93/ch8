#include "ch8_keyboard.h"

#include <assert.h>

#include <SDL.h>

#include "ch8_cpu.h"
#include "ch8_log.h"

ch8_key __SDLKeycodeToKeyRegister(SDL_Keycode keycode)
{
    switch (keycode)
    {
    case SDLK_x:
        return KEY_0;
    case SDLK_1:
        return KEY_1;
    case SDLK_2:
        return KEY_2;
    case SDLK_3:
        return KEY_3;
    case SDLK_q:
        return KEY_4;
    case SDLK_w:
        return KEY_5;
    case SDLK_e:
        return KEY_6;
    case SDLK_a:
        return KEY_7;
    case SDLK_s:
        return KEY_8;
    case SDLK_d:
        return KEY_9;
    case SDLK_z:
        return KEY_A;
    case SDLK_c:
        return KEY_B;
    case SDLK_4:
        return KEY_C;
    case SDLK_r:
        return KEY_D;
    case SDLK_f:
        return KEY_E;
    case SDLK_v:
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
