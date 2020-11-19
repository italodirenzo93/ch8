#ifndef __DISPLAY__
#define __DISPLAY__

#include <stdint.h>

struct ch8_cpu;

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 320

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32

typedef enum input_key
{
    INPUT_KEY_UNKNOWN = 0xFF,
    INPUT_KEY_UP = 0x08,
    INPUT_KEY_DOWN = 0x04,
    INPUT_KEY_LEFT = 0x06,
    INPUT_KEY_RIGHT = 0x02
} input_key;

#define CH8_KEYDOWN 1
#define CH8_KEYUP 0

void display_init();
void display_quit();
void display_event_loop(struct ch8_cpu *cpu);
void display_clear();
void draw_sprite(struct ch8_cpu *cpu, uint8_t x, uint8_t y, uint8_t h);

#endif
