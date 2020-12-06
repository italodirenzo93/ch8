#include "ch8_audio.h"

#include <SDL.h>

#include "ch8_log.h"

static SDL_AudioDeviceID audioDeviceId = 0;
static int sampleNr = 0;

#define AMPLITUDE 28000
#define SAMPLE_RATE 44100

#define INIT_CHECK if (audioDeviceId == 0) return

void __audioCallback(void* userdata, Uint8* stream, int len)
{
    Sint16 *buffer = (Sint16*)stream;
    len = len / 2; // 2 bytes per sample for AUDIO_S16SYS

    for (int i = 0; i < len; i++, sampleNr++) {
        double time = (double)sampleNr / (double)SAMPLE_RATE;
        buffer[i] = (Sint16)(AMPLITUDE * sin(2.0f * M_PI * 441.0f * time)); // render 441 HZ sine wave
    }
}

int ch8_audioInit()
{
    if (audioDeviceId != 0) {
        return;
    }

    SDL_AudioSpec want, have;
    SDL_AudioDeviceID dev;

    SDL_memset(&want, 0, sizeof(want)); /* or SDL_zero(want) */
    want.freq = 44100;
    want.format = AUDIO_S16SYS;
    want.channels = 1;
    want.samples = 2048;
    want.callback = __audioCallback; /* you wrote this function elsewhere -- see SDL_AudioSpec for details */

    audioDeviceId = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
    if (audioDeviceId == 0) {
        ch8_logError("Unable to find a usable audio device");
        return 1;
    }

    return 0;
}

void ch8_audioQuit()
{
    INIT_CHECK;
    SDL_CloseAudioDevice(audioDeviceId);
}

void ch8_audioUpdate(const ch8_cpu* cpu)
{
    INIT_CHECK;
    if (cpu->soundTimer > 0) {
        SDL_QueueAudio(audioDeviceId, NULL, 0);
    }
}
