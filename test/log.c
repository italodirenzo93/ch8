#include "ch8_cpu.h"
#include "log.h"
#include "keyboard.h"

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
    cpu->keypad[KEY_2] = CH8_KEYSTATE_DOWN;
    (*key) = KEY_2;
    return 0;
}

bool is_key_up(const ch8_cpu* cpu, input_key key)
{
    return false;
}

bool is_key_down(const ch8_cpu* cpu, input_key key)
{
    return false;
}

void display_write_fb()
{
}
