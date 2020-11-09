#ifndef __CH8_CPU__
#define __CH8_CPU__

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

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

void ch8_init(ch8_cpu **pcpu);
void ch8_quit(ch8_cpu *cpu);
void ch8_load_rom(ch8_cpu *cpu, uint8_t *program, size_t size);
bool ch8_load_rom_file(ch8_cpu *cpu, const char *file);
uint16_t ch8_next_opcode(ch8_cpu *cpu);
void ch8_exec_opcode(ch8_cpu *cpu, uint16_t opcode);

/* OPCODE TABLE */
#define CH8_OPCODE_MSB_CALL 0x0000
#define CH8_OPCODE_MSB_GOTO 0x1000
#define CH8_OPCODE_MSB_CALLSUB 0x2000
#define CH8_OPCODE_MSB_EQUAL 0x3000
#define CH8_OPCODE_MSB_NEQUAL 0x4000
#define CH8_OPCODE_MSB_EQUALVAR 0x5000
#define CH8_OPCODE_MSB_SET 0x6000
#define CH8_OPCODE_MSB_ADD 0x7000
#define CH8_OPCODE_MSB_SETVAR 0x8000

#define CH8_OPCODE_DISPLAY_CLEAR 0x00E0
#define CH8_OPCODE_RETURN 0x00EE

#endif
