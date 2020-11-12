#ifndef __DISPLAY__
#define __DISPLAY__

#include <stdint.h>

void display_init();
void display_quit();
void begin_frame(uint8_t *pixels);
void end_frame();
void draw_display();

#endif
