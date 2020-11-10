#include "ch8_gpu.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void ch8_init_gpu(ch8_gpu **pgpu)
{
    assert(pgpu != NULL);

    ch8_gpu *gpu = (ch8_gpu *)malloc(sizeof(ch8_gpu));
    memset(gpu->framebuffer, false, CH8_FRAMEBUFFER_SIZE);

    (*pgpu) = gpu;
}

void ch8_quit_gpu(ch8_gpu *gpu)
{
    assert(gpu != NULL);
    free(gpu);
}

void ch8_display_clear(ch8_gpu *gpu)
{
    assert(gpu != NULL);
    memset(gpu->framebuffer, 0, CH8_FRAMEBUFFER_SIZE);
    printf("Display clear\n");
}
