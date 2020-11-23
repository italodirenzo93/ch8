#include <assert.h>
#include <SDL_events.h>

#include "ch8_cpu.h"
#include "log.h"
#include "input.h"

bool is_key_down(ch8_cpu *cpu, input_key key)
{
    assert(cpu != NULL);
    if (key == INPUT_KEY_UNKNOWN) {
        return false;
    }
    return cpu->keypad[key] == CH8_KEYDOWN;
}

bool is_key_up(ch8_cpu *cpu, input_key key)
{
    assert(cpu != NULL);
    if (key == INPUT_KEY_UNKNOWN) {
        return false;
    }
    return cpu->keypad[key] == CH8_KEYUP;
}

void set_key_down(ch8_cpu *cpu, input_key key)
{
    assert(cpu != NULL);
    if (key != INPUT_KEY_UNKNOWN) {
        cpu->keypad[key] = CH8_KEYDOWN;
    }
}

void set_key_up(ch8_cpu *cpu, input_key key)
{
    assert(cpu != NULL);
    if (key != INPUT_KEY_UNKNOWN) {
        cpu->keypad[key] = CH8_KEYDOWN;
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
    
    (*key) = _scancode_to_key_register(event.key.keysym.scancode);
    if ((*key) != INPUT_KEY_UNKNOWN) {
        set_key_down(cpu, (*key));
    }
    
    return 0;
}
