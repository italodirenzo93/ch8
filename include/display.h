#ifndef __DISPLAY__
#define __DISPLAY__

#include <stdint.h>

struct ch8_cpu;

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 320

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32

void display_init();
void display_quit();
void display_poll_events();
void display_clear();
void draw_sprite(struct ch8_cpu *cpu, uint8_t x, uint8_t y, uint8_t h);

#endif
