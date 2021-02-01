#ifndef __OPCODES_H__
#define __OPCODES_H__

#include "ch8_cpu.h"

#ifdef __cplusplus
extern "C"
{
#endif

void ch8_op_ReturnFromSub(ch8_cpu *cpu);
void ch8_op_JumpTo(ch8_cpu *cpu, u16 opcode);
void ch8_op_ClearDisplay(ch8_cpu *cpu);

void ch8_op_CallSub(ch8_cpu *cpu, u16 opcode);

void ch8_op_SkipEquals(ch8_cpu *cpu, u16 opcode);
void ch8_op_SkipNotEquals(ch8_cpu *cpu, u16 opcode);
void ch8_op_SkipVXEqualsVY(ch8_cpu *cpu, u16 opcode);
void ch8_op_SkipVXNotEqualsVY(ch8_cpu *cpu, u16 opcode);

void ch8_op_Set(ch8_cpu *cpu, u16 opcode);
void ch8_op_Add(ch8_cpu *cpu, u16 opcode);

void ch8_op_Assign(ch8_cpu *cpu, u16 opcode);
void ch8_op_LogicalOr(ch8_cpu *cpu, u16 opcode);
void ch8_op_LogicalAnd(ch8_cpu *cpu, u16 opcode);
void ch8_op_LogicalXor(ch8_cpu *cpu, u16 opcode);
void ch8_op_AddAssign(ch8_cpu *cpu, u16 opcode);
void ch8_op_SubtractAssign(ch8_cpu *cpu, u16 opcode);
void ch8_op_BitshiftRight(ch8_cpu *cpu, u16 opcode);
void ch8_op_SubtractAssignReverse(ch8_cpu *cpu, u16 opcode);
void ch8_op_BitshiftLeft(ch8_cpu *cpu, u16 opcode);

void ch8_op_SetIndex(ch8_cpu *cpu, u16 opcode);
void ch8_op_JumpOffset(ch8_cpu *cpu, u16 opcode);
void ch8_op_BitwiseRandom(ch8_cpu *cpu, u16 opcode);
void ch8_op_DrawSprite(ch8_cpu *cpu, u16 opcode);

void ch8_op_KeyEquals(ch8_cpu *cpu, u16 opcode);
void ch8_op_KeyNotEquals(ch8_cpu *cpu, u16 opcode);

// FX07
void ch8_op_ReadDelayTimer(ch8_cpu *cpu, u16 opcode);
// FX0A
void ch8_op_KeyWait(ch8_cpu *cpu, u16 opcode);
// FX15
void ch8_op_SetDelayTimer(ch8_cpu *cpu, u16 opcode);
// FX18
void ch8_op_SetSoundTimer(ch8_cpu *cpu, u16 opcode);
// FX1E
void ch8_op_AddToIndex(ch8_cpu *cpu, u16 opcode);
// FX29
void ch8_op_SetFontChar(ch8_cpu *cpu, u16 opcode);
// FX33
void ch8_op_StoreBinaryCodedDecimal(ch8_cpu *cpu, u16 opcode);
// FX55
void ch8_op_Store(ch8_cpu *cpu, u16 opcode);
// FX65
void ch8_op_Load(ch8_cpu *cpu, u16 opcode);

#ifdef __cplusplus
}
#endif

#endif
