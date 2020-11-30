#ifndef __DISPLAY__
#define __DISPLAY__

#include "ch8_cpu.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 320

int display_init();
void display_quit();
void display_clear();
void display_present(const ch8_cpu *cpu);
void display_event_loop(ch8_cpu *cpu);
void display_write_fb(const ch8_cpu *cpu);

#endif
