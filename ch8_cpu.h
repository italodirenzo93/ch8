#ifndef __CH8_CPU__
#define __CH8_CPU__

#include <stdlib.h>
#include <stdint.h>

#define MEM_SIZE 4096
#define STACK_SIZE 48
#define NUM_REGISTERS 16

#define MAX_PROGRAM_SIZE 3232

#define PROGRAM_START_OFFSET 512
#define CALL_STACK_OFFSET 3744
#define DISPLAY_REFRESH_OFFSET 3840

struct __ch8_cpu
{
    uint8_t memory[MEM_SIZE];
    uint16_t I;
    uint8_t V[NUM_REGISTERS]; /* data registers */
    uint16_t PC;
    uint8_t stack[STACK_SIZE];
};

typedef struct __ch8_cpu ch8_cpu;

ch8_cpu *ch8_init();
void ch8_quit(ch8_cpu *cpu);
void ch8_load_rom(ch8_cpu *cpu, uint8_t *program, size_t size);

/* OPCODE TABLE */
#define CH8_OPCODE_MSB_CALL 0
#define CH8_OPCODE_MSB_GOTO 1
#define CH8_OPCODE_MSB_CALLSUB 2
#define CH8_OPCODE_MSB_EQUAL 3
#define CH8_OPCODE_MSB_NEQUAL 4
#define CH8_OPCODE_MSB_EQUALVAR 5
#define CH8_OPCODE_MSB_SET 6
#define CH8_OPCODE_MSB_ADD 7
#define CH8_OPCODE_MSB_SETVAR 8

#define CH8_OPCODE_DISPLAY_CLEAR 0x0E0
#define CH8_OPCODE_RETURN 0x0EE

#endif
