#ifndef __CH8_CPU__
#define __CH8_CPU__

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL_atomic.h>

#define CH8_MEM_SIZE 4096
#define CH8_STACK_SIZE 48
#define CH8_NUM_REGISTERS 16

#define CH8_MAX_PROGRAM_SIZE 3232
#define CH8_DISPLAY_SIZE 2048 /* 64x32 px */

#define CH8_PROGRAM_START_OFFSET 512
#define CH8_CALL_STACK_OFFSET 3744
#define CH8_DISPLAY_REFRESH_OFFSET 3840

#define PC_STEP_SIZE 2

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32

typedef struct ch8_cpu
{
    uint8_t memory[CH8_MEM_SIZE];
    uint16_t I;
    uint8_t V[CH8_NUM_REGISTERS]; /* data registers */
    uint16_t PC;
    uint8_t stack[CH8_STACK_SIZE];
    uint8_t display[CH8_DISPLAY_SIZE];
    SDL_atomic_t delayTimer;
    SDL_atomic_t soundTimer;
    uint8_t keypad[16];
    bool running;
} ch8_cpu;

int ch8_init(ch8_cpu **pcpu);
void ch8_quit(ch8_cpu **pcpu);
void ch8_reset(ch8_cpu *cpu);
void ch8_load_rom(ch8_cpu *cpu, const uint8_t *program, size_t size);
bool ch8_load_rom_file(ch8_cpu *cpu, const char *file);
uint16_t ch8_next_opcode(ch8_cpu *cpu);
bool ch8_exec_opcode(ch8_cpu *cpu);

#endif
