#ifndef __LOG_H__
#define __LOG_H__

#ifdef __cplusplus
extern "C"
{
#endif

int log_init();

void log_critical(const char *fmt, ...);
void log_error(const char *fmt, ...);
void log_warning(const char *fmt, ...);
void log_info(const char *fmt, ...);
void log_debug(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif
