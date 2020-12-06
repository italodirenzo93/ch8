#ifndef __CH8_CPU_H__
#define __CH8_CPU_H__

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define CH8_MEM_SIZE 4096
#define CH8_STACK_SIZE 64
#define CH8_NUM_REGISTERS 16
#define CH8_MAX_PROGRAM_SIZE 3232
#define CH8_NUM_KEYS 16

#define CH8_PROGRAM_START_OFFSET 0x200
#define CH8_CALL_STACK_OFFSET 0xEA0
#define CH8_DISPLAY_REFRESH_OFFSET 0xF00

#define CH8_PC_STEP_SIZE 2

#define CH8_DISPLAY_WIDTH 64
#define CH8_DISPLAY_HEIGHT 32
#define CH8_DISPLAY_SIZE 256

typedef struct ch8_cpu
{
    uint8_t memory[CH8_MEM_SIZE];
    uint16_t *stack;
    uint8_t *framebuffer;

    uint8_t V[CH8_NUM_REGISTERS]; /* data registers */

    uint16_t index;
    uint16_t programCounter;
    uint8_t stackPointer;

    uint8_t delayTimer;
    uint8_t soundTimer;

    bool keypad[CH8_NUM_KEYS];
    bool drawFlag;
} ch8_cpu;

void ch8_reset(ch8_cpu *cpu);
void ch8_loadRomData(ch8_cpu *cpu, const uint8_t *program, size_t size);
bool ch8_loadRomFile(ch8_cpu *cpu, const char *file);
uint16_t ch8_nextOpcode(ch8_cpu *cpu);
bool ch8_clockCycle(ch8_cpu *cpu, float elapsed_ms);

bool ch8_getPixel(const ch8_cpu *cpu, int x, int y);
void ch8_setPixel(ch8_cpu *cpu, int x, int y, bool on);

#ifdef __cplusplus
}
#endif

#endif
