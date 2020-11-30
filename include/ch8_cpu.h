#ifndef __CH8_CPU__
#define __CH8_CPU__

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define CH8_MEM_SIZE 4096
#define CH8_STACK_SIZE 64
#define CH8_NUM_REGISTERS 16
#define CH8_MAX_PROGRAM_SIZE 3232
#define CH8_NUM_KEYS 16

#define CH8_PROGRAM_START_OFFSET 0x200
#define CH8_CALL_STACK_OFFSET 3744
#define CH8_DISPLAY_REFRESH_OFFSET 3840

#define CH8_PC_STEP_SIZE 2

#define CH8_DISPLAY_WIDTH 64
#define CH8_DISPLAY_HEIGHT 32

typedef enum ch8_pixel_t
{
    CH8_PIXEL_ON = 1,
    CH8_PIXEL_OFF = 0
} ch8_pixel_t;

typedef enum ch8_keystate_t
{
    CH8_KEYSTATE_UP = 0,
    CH8_KEYSTATE_DOWN = 1
} ch8_keystate_t;

typedef struct ch8_cpu
{
    uint8_t memory[CH8_MEM_SIZE];
    uint16_t index_register;
    uint8_t V[CH8_NUM_REGISTERS]; /* data registers */
    uint16_t program_counter;
    uint16_t stack[CH8_STACK_SIZE];
    uint8_t stack_pointer;
    ch8_pixel_t framebuffer[CH8_DISPLAY_WIDTH * CH8_DISPLAY_HEIGHT];
    uint8_t delay_timer;
    uint8_t sound_timer;
    ch8_keystate_t keypad[CH8_NUM_KEYS];
    bool running;
} ch8_cpu;

void ch8_reset(ch8_cpu *cpu);
void ch8_load_rom(ch8_cpu *cpu, const uint8_t *program, size_t size);
bool ch8_load_rom_file(ch8_cpu *cpu, const char *file);
uint16_t ch8_next_opcode(ch8_cpu *cpu);
bool ch8_clock_cycle(ch8_cpu *cpu);
void ch8_update_timers(ch8_cpu *cpu, float elapsed);

#endif
