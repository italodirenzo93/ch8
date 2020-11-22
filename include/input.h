#ifndef __INPUT__
#define __INPUT__

#include <stdbool.h>
#include <SDL_scancode.h>

struct ch8_cpu;

typedef enum input_key
{
    INPUT_KEY_UNKNOWN = 0xFF,
    INPUT_KEY_UP = 0x08,
    INPUT_KEY_DOWN = 0x04,
    INPUT_KEY_LEFT = 0x06,
    INPUT_KEY_RIGHT = 0x02
} input_key;

#define CH8_KEYDOWN 1
#define CH8_KEYUP 0

static inline input_key _scancode_to_key_register(SDL_Scancode scancode)
{
    switch (scancode)
    {
    default:
        return INPUT_KEY_UNKNOWN;
    case SDL_SCANCODE_UP:
        return INPUT_KEY_UP;
    case SDL_SCANCODE_DOWN:
        return INPUT_KEY_DOWN;
    case SDL_SCANCODE_LEFT:
        return INPUT_KEY_LEFT;
    case SDL_SCANCODE_RIGHT:
        return INPUT_KEY_RIGHT;
    }
}

bool is_key_down(struct ch8_cpu *cpu, input_key key);
bool is_key_up(struct ch8_cpu *cpu, input_key key);

void set_key_down(struct ch8_cpu *cpu, input_key key);
void set_key_up(struct ch8_cpu *cpu, input_key key);

int await_keypress(struct ch8_cpu *cpu, input_key *key);

#endif
