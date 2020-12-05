#include "ch8_cpu.h"
#include "ch8_log.h"
#include "ch8_keyboard.h"

void ch8_logError(const char* fmt, ...)
{
}

void ch8_logWarning(const char* fmt, ...)
{
}

void ch8_logInfo(const char* fmt, ...)
{
}

void ch8_logDebug(const char* fmt, ...)
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

void ch8_displayWriteFb(const ch8_cpu* cpu)
{
}
