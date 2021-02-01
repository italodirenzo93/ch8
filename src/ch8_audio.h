#ifndef __CH8_AUDIO_H__
#define __CH8_AUDIO_H__

#include "ch8_cpu.h"

#ifdef __cplusplus
extern "C"
{
#endif

int ch8_audioInit();
void ch8_audioQuit();
void ch8_audioUpdate(const ch8_cpu* cpu);

#ifdef __cplusplus
}
#endif

#endif
