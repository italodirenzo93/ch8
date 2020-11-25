#include "ch8_cpu.h"
#include "log.h"
#include "input.h"

int log_init()
{
    return 0;
}

void log_error(const char* fmt, ...)
{
}

void log_warning(const char* fmt, ...)
{
}

void log_info(const char* fmt, ...)
{
}

void log_debug(const char* fmt, ...)
{
}

int await_keypress(ch8_cpu* cpu, input_key* key)
{
    cpu->keypad[INPUT_KEY_UP] = CH8_KEYSTATE_DOWN;
    (*key) = INPUT_KEY_UP;
}

void display_fb_copy(ch8_cpu* cpu)
{
}

bool is_key_up(ch8_cpu* cpu, input_key key)
{
    return false;
}

bool is_key_down(ch8_cpu* cpu, input_key key)
{
    return false;
}
