#ifndef __INPUT__
#define __INPUT__

#include <stdbool.h>
#include <SDL_scancode.h>

#include "ch8_cpu.h"

typedef enum input_key
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
} input_key;

input_key sdl_scancode_to_key_register(SDL_Scancode scancode);

bool is_key_down(const ch8_cpu *cpu, input_key key);
bool is_key_up(const ch8_cpu *cpu, input_key key);

void set_key_down(ch8_cpu *cpu, input_key key);
void set_key_up(ch8_cpu *cpu, input_key key);

int await_keypress(ch8_cpu *cpu, input_key *key);

#endif
