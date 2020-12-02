#include <iostream>
#include <SDL.h>

#include "log.hpp"

namespace ch8
{
    namespace log
    {
        static constexpr int LogCategory = SDL_LOG_CATEGORY_APPLICATION;

        int init()
        {
            SDL_LogSetPriority(LogCategory, SDL_LOG_PRIORITY_DEBUG);
            //SDL_LogSetOutputFunction(_logging_function, NULL);
            //freopen("log.txt", "a", stderr);
            return 0;
        }

        void critical(const char* fmt, ...)
        {
            va_list va;
            va_start(va, fmt);
            SDL_LogMessageV(LogCategory, SDL_LOG_PRIORITY_CRITICAL, fmt, va);
            va_end(va);
        }

        void error(const char* fmt, ...)
        {
            va_list va;
            va_start(va, fmt);
            SDL_LogMessageV(LogCategory, SDL_LOG_PRIORITY_ERROR, fmt, va);
            va_end(va);
        }

        void warning(const char* fmt, ...)
        {
            va_list va;
            va_start(va, fmt);
            SDL_LogMessageV(LogCategory, SDL_LOG_PRIORITY_WARN, fmt, va);
            va_end(va);
        }

        void info(const char* fmt, ...)
        {
            va_list va;
            va_start(va, fmt);
            SDL_LogMessageV(LogCategory, SDL_LOG_PRIORITY_INFO, fmt, va);
            va_end(va);
        }

        void debug(const char* fmt, ...)
        {
            va_list va;
            va_start(va, fmt);
            SDL_LogMessageV(LogCategory, SDL_LOG_PRIORITY_DEBUG, fmt, va);
            va_end(va);
        }
    }
}
