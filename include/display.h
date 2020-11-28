#ifndef __DISPLAY__
#define __DISPLAY__

struct ch8_cpu;

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 320

int display_init();
void display_quit();
void display_clear();
void display_present(const struct ch8_cpu* cpu);
void display_event_loop(struct ch8_cpu *cpu);
//void display_write_fb(const struct ch8_cpu *cpu, uint8_t x, uint8_t, uint8_t n);

#endif
