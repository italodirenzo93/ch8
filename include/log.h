#ifndef __LOGGING__
#define __LOGGING__

#include <SDL.h>

int log_init();

#define log_error(fmt, ...) SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, fmt, ##__VA_ARGS__)
#define log_warning(fmt, ...) SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, fmt, ##__VA_ARGS__)
#define log_info(fmt, ...) SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, fmt, ##__VA_ARGS__)
#define log_debug(fmt, ...) SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, fmt, ##__VA_ARGS__)

//void log_error(const char *fmt, ...);
//void log_warning(const char *fmt, ...);
//void log_info(const char *fmt, ...);
//void log_debug(const char *fmt, ...);

#endif
