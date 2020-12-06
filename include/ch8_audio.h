#ifndef __CH8_AUDIO_H__
#define __CH8_AUDIO_H__

#ifdef __cplusplus
extern "C"
{
#endif

int ch8_audioInit();
void ch8_audioQuit();

void ch8_beepStart();
void ch8_beepStop();

#ifdef __cplusplus
}
#endif

#endif
