#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "opcodes.h"
#include "display.h"
#include "ch8_cpu.h"

void ch8_op_return(ch8_cpu *cpu)
{
    assert(cpu != NULL);
    printf("Return from subroutine...\n");
}

void ch8_op_jumpto(ch8_cpu *cpu, uint16_t opcode)
{
    uint16_t addr = opcode & 0x0FFF;
    printf("Jump to memory address %X\n", addr);
    cpu->PC = addr;
}

void ch8_op_display_clear(ch8_cpu *cpu)
{
    printf("Clear display\n");
    display_clear();
}

void ch8_op_callsub(ch8_cpu *cpu, uint16_t opcode)
{
    uint16_t addr = opcode & 0x0FFF;
    printf("Call subroutine at address %X\n", addr);
}

void ch8_op_cond_eq(ch8_cpu *cpu, uint16_t opcode)
{
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t operand = opcode & 0x00FF;
    printf("IF conditional (V[%d] == %d)\n", vx, operand);
    if (cpu->V[vx] == operand)
    {
        // Skip the next instruction
        cpu->PC += 2;
    }
}

void ch8_op_cond_neq(ch8_cpu *cpu, uint16_t opcode)
{
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t operand = opcode & 0x00FF;
    printf("IF conditional (V[%d] != %d)\n", vx, operand);
    if (cpu->V[vx] != operand)
    {
        // Skip the next instruction
        cpu->PC += 2;
    }
}

void ch8_op_cond_vx_eq_vy(ch8_cpu *cpu, uint16_t opcode)
{
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t vy = (opcode & 0x00F0) >> 4;
    printf("IF conditional (V[%d] == V[%d])\n", vx, vy);
    if (cpu->V[vx] == cpu->V[vy])
    {
        // Skip the next instruction
        cpu->PC += 2;
    }
}

void ch8_op_cond_vx_neq_vy(ch8_cpu *cpu, uint16_t opcode)
{
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t vy = (opcode & 0x00F0) >> 4;
    printf("IF conditional (V[%d] != V[%d])\n", vx, vy);
    if (cpu->V[vx] != cpu->V[vy])
    {
        // Skip the next instruction
        cpu->PC += 2;
    }
}

void ch8_op_const_set(ch8_cpu *cpu, uint16_t opcode)
{
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t value = opcode & 0x00FF;
    printf("SET register V[%d] = %d\n", vx, value);
    cpu->V[vx] = value;
}

void ch8_op_const_add(ch8_cpu *cpu, uint16_t opcode)
{
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t operand = opcode & 0x00FF;
    printf("ADD %d to register V[%d]\n", operand, vx);
    cpu->V[vx] += operand;
}

void ch8_op_assign(ch8_cpu *cpu, uint16_t opcode)
{
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t vy = (opcode & 0x00F0) >> 4;

    printf("SET V[%d] = V[%d]\n", vx, vy);
    cpu->V[vx] = cpu->V[vy];
}

void ch8_op_or(ch8_cpu *cpu, uint16_t opcode)
{
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t vy = (opcode & 0x00F0) >> 4;

    printf("OR V[%d] = (V[%d] | V[%d])\n", vx, vx, vy);
    cpu->V[vx] = cpu->V[vx] | cpu->V[vy];
}

void ch8_op_and(ch8_cpu *cpu, uint16_t opcode)
{
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t vy = (opcode & 0x00F0) >> 4;

    printf("OR V[%d] = (V[%d] & V[%d])\n", vx, vx, vy);
    cpu->V[vx] = cpu->V[vx] & cpu->V[vy];
}

void ch8_op_xor(ch8_cpu *cpu, uint16_t opcode)
{
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t vy = (opcode & 0x00F0) >> 4;

    printf("OR V[%d] = (V[%d] ^ V[%d])\n", vx, vx, vy);
    cpu->V[vx] = cpu->V[vx] ^ cpu->V[vy];
}

void ch8_op_add_vx_to_vy(ch8_cpu *cpu, uint16_t opcode)
{
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t vy = (opcode & 0x00F0) >> 4;
    uint16_t result = cpu->V[vx] + cpu->V[vy];
    if (result > 0xFF)
    {
        // set the "carry" flag
        cpu->V[vx] = 0xFF;
        cpu->V[0xF] = 1;
    }
    else
    {
        cpu->V[vx] = result;
        cpu->V[0xF] = 0;
    }
}

void ch8_op_sub_vy_from_vx(ch8_cpu *cpu, uint16_t opcode)
{
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t vy = (opcode & 0x00F0) >> 4;
    uint16_t result = cpu->V[vx] - cpu->V[vy];
    if (result > 0xFF)
    {
        cpu->V[vx] = 0;
        cpu->V[0xF] = 0;
    }
    else
    {
        cpu->V[vx] = result;
        cpu->V[0xF] = 1;
    }
}

void ch8_op_set_addr(ch8_cpu *cpu, uint16_t opcode)
{
    uint16_t addr = opcode & 0x0FFF;
    printf("SET I = %d\n", addr);
    cpu->I = addr;
}

void ch8_jump_to_addr_plus_v0(ch8_cpu *cpu, uint16_t opcode)
{
    uint16_t addr = opcode & 0x0FFF;
    printf("JUMP to %d + V[0]\n", addr);
    cpu->PC = addr + cpu->V[0];
}

void ch8_op_bitwise_rand(ch8_cpu *cpu, uint16_t opcode)
{
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t nn = (opcode & 0x00FF);
    printf("RAND V[%d] = rand() & %d\n", vx, nn);
    cpu->V[vx] = rand() & nn;
}

void ch8_op_draw_sprite(ch8_cpu *cpu, uint16_t opcode)
{
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t vy = (opcode & 0x00F0) >> 4;
    uint8_t n = (opcode & 0x000F);
    draw_sprite(cpu, vx, vy, n);
}

void ch8_op_keyop_eq(ch8_cpu *cpu, uint16_t opcode)
{
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t key = cpu->keypad[vx];
    printf("KEY check if keypad[%d] is down\n", key);
    if (key == CH8_KEYDOWN)
    {
        cpu->PC += 2;
    }
}

void ch8_op_keyop_neq(ch8_cpu *cpu, uint16_t opcode)
{
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t key = cpu->keypad[vx];
    printf("KEY check if keypad[%d] is up\n", key);
    if (key == CH8_KEYUP)
    {
        cpu->PC += 2;
    }
}
