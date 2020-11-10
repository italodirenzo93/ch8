#ifndef __CH8_GPU__
#define __CH8_GPU__

#include <stdbool.h>

#define CH8_FRAMEBUFFER_SIZE 2048 /* 64x32 px */

struct __ch8_gpu
{
    bool framebuffer[CH8_FRAMEBUFFER_SIZE];
};

typedef struct __ch8_gpu ch8_gpu;

void ch8_init_gpu(ch8_gpu **pgpu);
void ch8_quit_gpu(ch8_gpu *gpu);
void ch8_display_clear(ch8_gpu *gpu);

#endif
