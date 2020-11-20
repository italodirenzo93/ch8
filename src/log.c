#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <SDL.h>

#include "log.h"

#define LOG_CATEGORY SDL_LOG_CATEGORY_APPLICATION

static const char* _get_priority_str(SDL_LogPriority priority)
{
    switch (priority)
    {
    default:
    case SDL_LOG_PRIORITY_CRITICAL:
        return "CRIT";
    case SDL_LOG_PRIORITY_ERROR:
        return "ERR";
    case SDL_LOG_PRIORITY_WARN:
        return "WARN";
    case SDL_LOG_PRIORITY_INFO:
        return "INFO";
    case SDL_LOG_PRIORITY_DEBUG:
        return "DBG";
    }
}

static void _logging_function(void *userdata, int category, SDL_LogPriority priority, const char *message)
{
    const char fmt[] = "[%s] %s\n";

    // Write to text file
    FILE *fp = NULL;
    fp = fopen("log.txt", "a");
    if (fp != NULL)
    {
        fprintf(fp, fmt, _get_priority_str(priority), message);
        fclose(fp);
        fp = NULL;
    }
    
    // Write to stderr
    fprintf(stderr, fmt, _get_priority_str(priority), message);
}

int log_init()
{
    SDL_LogSetPriority(LOG_CATEGORY, SDL_LOG_PRIORITY_DEBUG);
    //SDL_LogSetOutputFunction(_logging_function, NULL);
    //freopen("log.txt", "a", stderr);
    return 0;
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
