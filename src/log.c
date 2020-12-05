#include "log.h"

#include <stdio.h>
#include <SDL.h>

#define LOG_CATEGORY SDL_LOG_CATEGORY_APPLICATION

int log_init()
{
    SDL_LogSetPriority(LOG_CATEGORY, SDL_LOG_PRIORITY_DEBUG);
    //SDL_LogSetOutputFunction(_logging_function, NULL);
    //freopen("log.txt", "a", stderr);
    return 0;
}

void log_critical(const char* fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    SDL_LogMessageV(LOG_CATEGORY, SDL_LOG_PRIORITY_CRITICAL, fmt, va);
    va_end(va);
}

void log_error(const char* fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    SDL_LogMessageV(LOG_CATEGORY, SDL_LOG_PRIORITY_ERROR, fmt, va);
    va_end(va);
}

void log_warning(const char* fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    SDL_LogMessageV(LOG_CATEGORY, SDL_LOG_PRIORITY_WARN, fmt, va);
    va_end(va);
}

void log_info(const char* fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    SDL_LogMessageV(LOG_CATEGORY, SDL_LOG_PRIORITY_INFO, fmt, va);
    va_end(va);
}

void log_debug(const char* fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    SDL_LogMessageV(LOG_CATEGORY, SDL_LOG_PRIORITY_DEBUG, fmt, va);
    va_end(va);
}
