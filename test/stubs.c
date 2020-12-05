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

int ch8_awaitKeyPress(ch8_cpu* cpu, ch8_key* key)
{
    cpu->keypad[KEY_2] = true;
    (*key) = KEY_2;
    return 0;
}

bool ch8_isKeyUp(const ch8_cpu* cpu, ch8_key key)
{
    return false;
}

bool ch8_isKeyDown(const ch8_cpu* cpu, ch8_key key)
{
    return false;
}

void ch8_displayWriteFb()
{
}
