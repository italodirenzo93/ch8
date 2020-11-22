#ifndef __OPCODES__
#define __OPCODES__

#include <stdint.h>

struct ch8_cpu;

void ch8_op_return(struct ch8_cpu *cpu);
void ch8_op_jumpto(struct ch8_cpu *cpu, uint16_t opcode);
void ch8_op_display_clear(struct ch8_cpu *cpu);

void ch8_op_callsub(struct ch8_cpu *cpu, uint16_t opcode);

void ch8_op_cond_eq(struct ch8_cpu *cpu, uint16_t opcode);
void ch8_op_cond_neq(struct ch8_cpu *cpu, uint16_t opcode);
void ch8_op_cond_vx_eq_vy(struct ch8_cpu *cpu, uint16_t opcode);
void ch8_op_cond_vx_neq_vy(struct ch8_cpu *cpu, uint16_t opcode);

void ch8_op_const_set(struct ch8_cpu *cpu, uint16_t opcode);
void ch8_op_const_add(struct ch8_cpu *cpu, uint16_t opcode);

void ch8_op_assign(struct ch8_cpu *cpu, uint16_t opcode);
void ch8_op_or(struct ch8_cpu *cpu, uint16_t opcode);
void ch8_op_and(struct ch8_cpu *cpu, uint16_t opcode);
void ch8_op_xor(struct ch8_cpu *cpu, uint16_t opcode);
void ch8_op_add_vx_to_vy(struct ch8_cpu *cpu, uint16_t opcode);
void ch8_op_sub_vy_from_vx(struct ch8_cpu *cpu, uint16_t opcode);
void ch8_op_bitshift_right_vx_to_vf(struct ch8_cpu *cpu, uint16_t opcode);
void ch8_op_set_vx_to_vx_sub_vy(struct ch8_cpu *cpu, uint16_t opcode);
void ch8_op_bitshift_left_vx_to_vf(struct ch8_cpu *cpu, uint16_t opcode);

void ch8_op_set_addr(struct ch8_cpu *cpu, uint16_t opcode);
void ch8_jump_to_addr_plus_v0(struct ch8_cpu *cpu, uint16_t opcode);
void ch8_op_bitwise_rand(struct ch8_cpu *cpu, uint16_t opcode);
void ch8_op_draw_sprite(struct ch8_cpu *cpu, uint16_t opcode);

void ch8_op_keyop_eq(struct ch8_cpu *cpu, uint16_t opcode);
void ch8_op_keyop_neq(struct ch8_cpu *cpu, uint16_t opcode);

// 0xFX07
void ch8_op_set_vx_to_delay_timer(struct ch8_cpu *cpu, uint16_t opcode);
// 0xFX0A
void ch8_op_await_keypress(struct ch8_cpu *cpu, uint16_t opcode);
// 0xFX15
void ch8_op_set_delay_timer_to_vx(struct ch8_cpu *cpu, uint16_t opcode);
// 0xFX18
void ch8_op_set_sound_timer_to_vx(struct ch8_cpu *cpu, uint16_t opcode);

#endif
