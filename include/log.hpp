#pragma once

namespace ch8
{
    namespace log
    {
        int init();

        void critical(const char* fmt, ...);
        void error(const char* fmt, ...);
        void warning(const char* fmt, ...);
        void info(const char* fmt, ...);
        void debug(const char* fmt, ...);
    }
}
