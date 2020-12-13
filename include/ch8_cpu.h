#ifndef __CPU_H__
#define __CPU_H__

#include <stdlib.h>
#include <stdbool.h>

#include "ch8_def.h"

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
    u8 memory[CH8_MEM_SIZE];
    u16 *stack;
    u8 *framebuffer;

    u8 V[CH8_NUM_REGISTERS]; /* data registers */

    u16 index;
    u16 programCounter;
    u8 stackPointer;

    u8 delayTimer;
    u8 soundTimer;

    bool keypad[CH8_NUM_KEYS];

    bool drawFlag;
    bool waitFlag;
    u8 waitReg;
} ch8_cpu;

void ch8_reset(ch8_cpu *cpu);
bool ch8_loadRomFile(ch8_cpu *cpu, const char *file);
u16 ch8_nextOpcode(ch8_cpu *cpu);
bool ch8_clockCycle(ch8_cpu *cpu, float elapsed_ms);

bool ch8_getPixel(const ch8_cpu *cpu, int x, int y);
void ch8_setPixel(ch8_cpu *cpu, int x, int y, bool on);

#ifdef __cplusplus
}
#endif

#endif
