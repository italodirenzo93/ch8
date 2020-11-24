#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "opcodes.h"
#include "display.h"
#include "ch8_cpu.h"
#include "log.h"
#include "input.h"

void ch8_op_return(ch8_cpu *cpu)
{
    assert(cpu != NULL);
    log_debug("Return from subroutine...\n");
    cpu->stack_pointer = 0;
}

void ch8_op_jumpto(ch8_cpu *cpu, uint16_t opcode)
{
    uint16_t addr = opcode & 0x0FFF;
    log_debug("Jump to memory address %X\n", addr);
    cpu->PC = addr;
}

void ch8_op_display_clear(ch8_cpu *cpu)
{
    log_debug("Clear display\n");
    
    int i;
    int length = CH8_DISPLAY_WIDTH * CH8_DISPLAY_HEIGHT;
    for (i = 0; i < length; i++) {
        cpu->display[i] = 0;
    }
    
    display_fb_copy(cpu);
}

void ch8_op_callsub(ch8_cpu *cpu, uint16_t opcode)
{
    uint16_t addr = opcode & 0x0FFF;
    log_debug("Call subroutine at address %X\n", addr);
}

void ch8_op_cond_eq(ch8_cpu *cpu, uint16_t opcode)
{
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t operand = opcode & 0x00FF;
    log_debug("IF conditional (V[%d] == %d)\n", vx, operand);
    if (cpu->V[vx] == operand)
    {
        // Skip the next instruction
        cpu->PC += CH8_PC_STEP_SIZE;
    }
}

void ch8_op_cond_neq(ch8_cpu *cpu, uint16_t opcode)
{
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t operand = opcode & 0x00FF;
    log_debug("IF conditional (V[%d] != %d)\n", vx, operand);
    if (cpu->V[vx] != operand)
    {
        // Skip the next instruction
        cpu->PC += CH8_PC_STEP_SIZE;
    }
}

void ch8_op_cond_vx_eq_vy(ch8_cpu *cpu, uint16_t opcode)
{
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t vy = (opcode & 0x00F0) >> 4;
    log_debug("IF conditional (V[%d] == V[%d])\n", vx, vy);
    if (cpu->V[vx] == cpu->V[vy])
    {
        // Skip the next instruction
        cpu->PC += CH8_PC_STEP_SIZE;
    }
}

void ch8_op_cond_vx_neq_vy(ch8_cpu *cpu, uint16_t opcode)
{
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t vy = (opcode & 0x00F0) >> 4;
    log_debug("IF conditional (V[%d] != V[%d])\n", vx, vy);
    if (cpu->V[vx] != cpu->V[vy])
    {
        // Skip the next instruction
        cpu->PC += CH8_PC_STEP_SIZE;
    }
}

void ch8_op_const_set(ch8_cpu *cpu, uint16_t opcode)
{
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t value = opcode & 0x00FF;
    log_debug("SET register V[%d] = %d\n", vx, value);
    cpu->V[vx] = value;
}

void ch8_op_const_add(ch8_cpu *cpu, uint16_t opcode)
{
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t operand = opcode & 0x00FF;
    log_debug("ADD %d to register V[%d]\n", operand, vx);
    cpu->V[vx] += operand;
}

void ch8_op_assign(ch8_cpu *cpu, uint16_t opcode)
{
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t vy = (opcode & 0x00F0) >> 4;

    log_debug("SET V[%d] = V[%d]\n", vx, vy);
    cpu->V[vx] = cpu->V[vy];
}

void ch8_op_or(ch8_cpu *cpu, uint16_t opcode)
{
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t vy = (opcode & 0x00F0) >> 4;

    log_debug("OR V[%d] = (V[%d] | V[%d])\n", vx, vx, vy);
    cpu->V[vx] = cpu->V[vx] | cpu->V[vy];
}

void ch8_op_and(ch8_cpu *cpu, uint16_t opcode)
{
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t vy = (opcode & 0x00F0) >> 4;

    log_debug("OR V[%d] = (V[%d] & V[%d])\n", vx, vx, vy);
    cpu->V[vx] = cpu->V[vx] & cpu->V[vy];
}

void ch8_op_xor(ch8_cpu *cpu, uint16_t opcode)
{
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t vy = (opcode & 0x00F0) >> 4;

    log_debug("OR V[%d] = (V[%d] ^ V[%d])\n", vx, vx, vy);
    cpu->V[vx] = cpu->V[vx] ^ cpu->V[vy];
}

void ch8_op_add_vx_to_vy(ch8_cpu *cpu, uint16_t opcode)
{
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t vy = (opcode & 0x00F0) >> 4;
    uint16_t result = cpu->V[vx] + cpu->V[vy];
    if (result > UINT8_MAX)
    {
        // set the "carry" flag
        cpu->V[vx] = UINT8_MAX;
        cpu->V[0xF] = 1;
    }
    else
    {
        cpu->V[vx] = (uint8_t) result;
        cpu->V[0xF] = 0;
    }
    
    log_debug("ADD V[%d] += V[%d] : V[0xF] = %d", vx, vy, cpu->V[0xF]);
}

void ch8_op_sub_vy_from_vx(ch8_cpu *cpu, uint16_t opcode)
{
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t vy = (opcode & 0x00F0) >> 4;
    uint16_t result = cpu->V[vx] - cpu->V[vy];
    if (result > UINT8_MAX)
    {
        cpu->V[vx] = 0;
        cpu->V[0xF] = 0;
    }
    else
    {
        cpu->V[vx] = (uint8_t) result;
        cpu->V[0xF] = 1;
    }
    
    log_debug("SUB V[%d] -= V[%d] : V[0xF] = %d", vx, vy, cpu->V[0xF]);
}

void ch8_op_bitshift_right_vx_to_vf(ch8_cpu *cpu, uint16_t opcode)
{
    uint8_t vx = (opcode & 0x0F00) >> 8;
    cpu->V[0xF] = cpu->V[vx] & 0x1;
    cpu->V[vx] >>= 1;
    log_debug("SHIFTR V[%d] >>= 1", vx);
}

void ch8_op_set_vx_to_vx_sub_vy(ch8_cpu *cpu, uint16_t opcode)
{
    uint16_t vx = (opcode & 0x0F00) >> 8;
    uint16_t vy = (opcode & 0x00F0) >> 8;
    cpu->V[0xF] = cpu->V[vy] < cpu->V[vx] ? 0x00 : 0x01;
    cpu->V[vx] = cpu->V[vy] - cpu->V[vx];
}

void ch8_op_bitshift_left_vx_to_vf(ch8_cpu *cpu, uint16_t opcode)
{
    uint8_t vx = (opcode & 0x0F00) >> 8;
    cpu->V[0xF] = (cpu->V[vx] >> 7) & 0x1;
    cpu->V[vx] <<= 1;
    log_debug("SHIFTL V[%d] >>= 1", vx);
}

void ch8_op_set_addr(ch8_cpu *cpu, uint16_t opcode)
{
    uint16_t addr = opcode & 0x0FFF;
    log_debug("SET I = %d\n", addr);
    cpu->I = addr;
}

void ch8_jump_to_addr_plus_v0(ch8_cpu *cpu, uint16_t opcode)
{
    uint16_t addr = opcode & 0x0FFF;
    log_debug("JUMP to %d + V[0]\n", addr);
    cpu->PC = addr + cpu->V[0];
}

void ch8_op_bitwise_rand(ch8_cpu *cpu, uint16_t opcode)
{
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t nn = (opcode & 0x00FF);
    log_debug("RAND V[%d] = rand() & %d\n", vx, nn);
    cpu->V[vx] = rand() & nn;
}

void ch8_op_draw_sprite(ch8_cpu *cpu, uint16_t opcode)
{
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t vy = (opcode & 0x00F0) >> 4;
    uint8_t n = (opcode & 0x000F);
    uint8_t startIdx = vy * CH8_DISPLAY_WIDTH + vx;
    
    uint8_t destX = vx + 8;
    uint8_t destY = vy + n;
    uint8_t destIdx = destY * CH8_DISPLAY_WIDTH + destX;
    
    int i, xor = 0;
    for (i = startIdx; i < destIdx; i++) {
        if (xor == 0) {
            xor = cpu->display[i] ^ 1;
        }
        cpu->display[i] = 1;
    }
    
    cpu->V[0xF] = xor;
    
    display_fb_copy(cpu);
}

void ch8_op_keyop_eq(ch8_cpu *cpu, uint16_t opcode)
{
    input_key key = (input_key) ((opcode & 0x0F00) >> 8);
    log_debug("KEY check if keypad[%d] is down\n", key);
    if (is_key_down(cpu, key)) {
        cpu->PC += CH8_PC_STEP_SIZE;
    }
}

void ch8_op_keyop_neq(ch8_cpu *cpu, uint16_t opcode)
{
    assert(cpu != NULL);
    input_key key = (input_key) ((opcode & 0x0F00) >> 8);
    log_debug("KEY check if keypad[%d] is up\n", key);
    if (is_key_up(cpu, key)) {
        cpu->PC += CH8_PC_STEP_SIZE;
    }
}

// 0xFX07
void ch8_op_set_vx_to_delay_timer(ch8_cpu *cpu, uint16_t opcode)
{
    uint16_t vx = (opcode & 0x0F00) >> 8;
    cpu->V[vx] = cpu->delay_timer > UINT8_MAX ? UINT8_MAX : cpu->delay_timer < 0 ? 0 : (uint8_t) cpu->delay_timer;
    log_debug("TIMER set V[%d] = delay timer val %d", vx, cpu->delay_timer);
}

// 0xFX0A
void ch8_op_await_keypress(ch8_cpu *cpu, uint16_t opcode)
{
    uint16_t vx = (opcode & 0x0F00) >> 8;
 
    input_key key;
    if (await_keypress(cpu, &key) != 0) {
        log_error("Error awaiting keypress");
        return;
    }
    
    if (key != INPUT_KEY_UNKNOWN) {
        cpu->V[vx] = (uint8_t) key;
    }
}

// 0xFX15
void ch8_op_set_delay_timer_to_vx(ch8_cpu *cpu, uint16_t opcode)
{
    uint16_t vx = (opcode & 0x0F00) >> 8;
    cpu->delay_timer = cpu->V[vx];
    
    log_debug("TIMER set delay timer to V[%d] (%d)", vx, cpu->delay_timer);
}

// 0xFX18
void ch8_op_set_sound_timer_to_vx(ch8_cpu *cpu, uint16_t opcode)
{
    uint16_t vx = (opcode & 0x0F00) >> 8;
    cpu->sound_timer = cpu->V[vx];

    log_debug("SOUND set sound timer to V[%d] (%d)", vx, cpu->sound_timer);
}

// 0xFX1E
void ch8_op_add_vx_to_I(ch8_cpu *cpu, uint16_t opcode)
{
    uint16_t vx = (opcode & 0x0F00) >> 8;
    cpu->I += cpu->V[vx];
}

// 0xFX29
void ch8_op_set_I_to_sprite_addr(ch8_cpu *cpu, uint16_t opcode)
{
    // TODO: This is probably wrong...
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t ch = cpu->V[vx];
    uint16_t i = ch * (4 * 5);
    cpu->I = cpu->display[i];
    log_debug("SET I = sprite_addr[%d] (%d * 4 * 5)", i, vx);
}

// 0xFX33
void ch8_op_store_bcd_of_vx(ch8_cpu *cpu, uint16_t opcode)
{
    STUBBED("opcode FX33 store BCD");
}

// 0xFX55
void ch8_op_store_v0_to_vx(ch8_cpu *cpu, uint16_t opcode)
{
    STUBBED("opcode FX55 store MEM");
}

// 0xFX65
void ch8_op_fill_v0_to_vx(ch8_cpu *cpu, uint16_t opcode)
{
    STUBBED("opcode FX65 fill MEM");
}
