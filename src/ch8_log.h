#ifndef __LOG_H__
#define __LOG_H__

#ifdef __cplusplus
extern "C"
{
#endif

int ch8_logInit();
void ch8_logQuit();

void ch8_logCritical(const char *fmt, ...);
void ch8_logError(const char *fmt, ...);
void ch8_logWarning(const char *fmt, ...);
void ch8_logInfo(const char *fmt, ...);
void ch8_logDebug(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif
