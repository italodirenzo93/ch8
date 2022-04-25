#include "ch8_log.h"

#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>

#define LOG_CATEGORY SDL_LOG_CATEGORY_APPLICATION

static bool initialized = false;
#define INIT_CHECK if (!initialized) return

int ch8_logInit()
{
    SDL_LogSetPriority(LOG_CATEGORY, SDL_LOG_PRIORITY_DEBUG);
    initialized = true;
    return 0;
}

void ch8_logQuit()
{
    initialized = false;
}

void ch8_logCritical(const char* fmt, ...)
{
    INIT_CHECK;
    va_list va;
    va_start(va, fmt);
    SDL_LogMessageV(LOG_CATEGORY, SDL_LOG_PRIORITY_CRITICAL, fmt, va);
    va_end(va);
}

void ch8_logError(const char* fmt, ...)
{
    INIT_CHECK;
    va_list va;
    va_start(va, fmt);
    SDL_LogMessageV(LOG_CATEGORY, SDL_LOG_PRIORITY_ERROR, fmt, va);
    va_end(va);
}

void ch8_logWarning(const char* fmt, ...)
{
    INIT_CHECK;
    va_list va;
    va_start(va, fmt);
    SDL_LogMessageV(LOG_CATEGORY, SDL_LOG_PRIORITY_WARN, fmt, va);
    va_end(va);
}

void ch8_logInfo(const char* fmt, ...)
{
    INIT_CHECK;
    va_list va;
    va_start(va, fmt);
    SDL_LogMessageV(LOG_CATEGORY, SDL_LOG_PRIORITY_INFO, fmt, va);
    va_end(va);
}

void ch8_logDebug(const char* fmt, ...)
{
    INIT_CHECK;
    va_list va;
    va_start(va, fmt);
    SDL_LogMessageV(LOG_CATEGORY, SDL_LOG_PRIORITY_DEBUG, fmt, va);
    va_end(va);
}
