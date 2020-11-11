#include "ch8_gpu.h"
#include "util.h"
#include "display.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>

void ch8_init_gpu(ch8_gpu **pgpu)
{
    assert(pgpu != NULL);

    ch8_gpu *gpu = (ch8_gpu *)ch8_malloc(sizeof(ch8_gpu));

    (*pgpu) = gpu;

    display_init();
}

void ch8_quit_gpu(ch8_gpu **gpu)
{
    assert(gpu != NULL);
    ch8_free((void **)gpu);

    display_quit();
}

void ch8_display_clear(ch8_gpu *gpu)
{
    assert(gpu != NULL);
    memset(gpu->framebuffer, 0, CH8_FRAMEBUFFER_SIZE);
    printf("Display clear\n");
}
