#ifndef __OPCODES__
#define __OPCODES__

#include "ch8_cpu.h"

void ch8_op_return(ch8_cpu *cpu);
void ch8_op_jumpto(ch8_cpu *cpu, uint16_t opcode);
void ch8_op_display_clear(ch8_cpu *cpu);

void ch8_op_callsub(ch8_cpu *cpu, uint16_t opcode);

void ch8_op_cond_eq(ch8_cpu *cpu, uint16_t opcode);
void ch8_op_cond_neq(ch8_cpu *cpu, uint16_t opcode);
void ch8_op_cond_vx_eq_vy(ch8_cpu *cpu, uint16_t opcode);
void ch8_op_cond_vx_neq_vy(ch8_cpu *cpu, uint16_t opcode);

void ch8_op_const_set(ch8_cpu *cpu, uint16_t opcode);
void ch8_op_const_add(ch8_cpu *cpu, uint16_t opcode);

void ch8_op_assign(ch8_cpu *cpu, uint16_t opcode);
void ch8_op_or(ch8_cpu *cpu, uint16_t opcode);
void ch8_op_and(ch8_cpu *cpu, uint16_t opcode);
void ch8_op_xor(ch8_cpu *cpu, uint16_t opcode);
void ch8_op_add_vx_to_vy(ch8_cpu *cpu, uint16_t opcode);
void ch8_op_sub_vy_from_vx(ch8_cpu *cpu, uint16_t opcode);
void ch8_op_bitshift_right_vx_to_vf(ch8_cpu *cpu, uint16_t opcode);
void ch8_op_set_vx_to_vx_sub_vy(ch8_cpu *cpu, uint16_t opcode);
void ch8_op_bitshift_left_vx_to_vf(ch8_cpu *cpu, uint16_t opcode);

void ch8_op_set_addr(ch8_cpu *cpu, uint16_t opcode);
void ch8_jump_to_addr_plus_v0(ch8_cpu *cpu, uint16_t opcode);
void ch8_op_bitwise_rand(ch8_cpu *cpu, uint16_t opcode);
void ch8_op_draw_sprite(ch8_cpu *cpu, uint16_t opcode);

void ch8_op_keyop_eq(ch8_cpu *cpu, uint16_t opcode);
void ch8_op_keyop_neq(ch8_cpu *cpu, uint16_t opcode);

// 0xFX07
void ch8_op_set_vx_to_delay_timer(ch8_cpu *cpu, uint16_t opcode);
// 0xFX0A
void ch8_op_await_keypress(ch8_cpu *cpu, uint16_t opcode);
// 0xFX15
void ch8_op_set_delay_timer_to_vx(ch8_cpu *cpu, uint16_t opcode);
// 0xFX18
void ch8_op_set_sound_timer_to_vx(ch8_cpu *cpu, uint16_t opcode);
// 0xFX1E
void ch8_op_add_vx_to_I(ch8_cpu *cpu, uint16_t opcode);
// 0xFX29
void ch8_op_set_I_to_sprite_addr(ch8_cpu *cpu, uint16_t opcode);
// 0xFX33
void ch8_op_store_bcd_of_vx(ch8_cpu *cpu, uint16_t opcode);
// 0xFX55
void ch8_op_store_v0_to_vx(ch8_cpu *cpu, uint16_t opcode);
// 0xFX65
void ch8_op_fill_v0_to_vx(ch8_cpu *cpu, uint16_t opcode);

#endif
