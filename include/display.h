#ifndef __DISPLAY__
#define __DISPLAY__

#include <stdint.h>

struct ch8_cpu;

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 320

int display_init();
void display_quit();
void display_event_loop(struct ch8_cpu *cpu);
void display_clear();
void display_present();
void display_fb_copy(struct ch8_cpu *cpu);

#endif
