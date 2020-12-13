#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include "ch8_cpu.h"

#include <stdbool.h>
#include <SDL_keycode.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum ch8_key
{
    KEY_UNKNOWN = 0xFF,
    KEY_0 = 0x0,
    KEY_1 = 0x1,
    KEY_2 = 0x2,
    KEY_3 = 0x3,
    KEY_4 = 0x4,
    KEY_5 = 0x5,
    KEY_6 = 0x6,
    KEY_7 = 0x7,
    KEY_8 = 0x8,
    KEY_9 = 0x9,
    KEY_A = 0xA,
    KEY_B = 0xB,
    KEY_C = 0xC,
    KEY_D = 0xD,
    KEY_E = 0xE,
    KEY_F = 0xF,
} ch8_key;

ch8_key __SDLKeycodeToKeyRegister(SDL_Keycode keycode);

bool ch8_isKeyDown(const ch8_cpu *cpu, ch8_key key);
bool ch8_isKeyUp(const ch8_cpu *cpu, ch8_key key);

void ch8_setKeyDown(ch8_cpu *cpu, ch8_key key);
void ch8_setKeyUp(ch8_cpu *cpu, ch8_key key);

#ifdef __cplusplus
}
#endif

#endif
