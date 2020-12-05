#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "ch8_cpu.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 320

int ch8_displayInit(void* handle);
void ch8_displayQuit();
void ch8_displayClear();
void ch8_displayPresent(const ch8_cpu *cpu);
void ch8_displayWriteFb(const ch8_cpu *cpu);

#ifdef __cplusplus
}
#endif

#endif
