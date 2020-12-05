#include "opcodes.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "display.h"
#include "ch8_cpu.h"
#include "log.h"
#include "keyboard.h"
#include "util.h"

// 0x00E0
void ch8_op_display_clear(ch8_cpu *cpu)
{
    assert(cpu != NULL);
    log_debug("[00E0] - Clear Display");

    for (int i = 0; i < CH8_DISPLAY_SIZE; i++) {
        cpu->framebuffer[i] = 0;
    }
}

// 0x00EE
void ch8_op_return(ch8_cpu* cpu)
{
    assert(cpu != NULL);
    log_debug("[00EE] - Return from sub-routine");

    cpu->programCounter = cpu->stack[--cpu->stackPointer];
}

// 0x1NNN
void ch8_op_jumpto(ch8_cpu* cpu, uint16_t opcode)
{
    assert(cpu != NULL);
    const uint16_t addr = opcode & 0x0FFF;
    log_debug("[1NNN] - Jump to address %X", addr);
    cpu->programCounter = addr;
}

// 0x2NNN
void ch8_op_callsub(ch8_cpu *cpu, uint16_t opcode)
{
    assert(cpu != NULL);

    const uint16_t addr = opcode & 0x0FFF;
    log_debug("[2NNN] - Call subroutine at address %X", addr);

    cpu->stack[cpu->stackPointer++] = cpu->programCounter;
    cpu->programCounter = addr;
}

// 0x3XKK
void ch8_op_cond_eq(ch8_cpu *cpu, uint16_t opcode)
{
    assert(cpu != NULL);
    const uint8_t x = (opcode & 0x0F00) >> 8;
    const uint8_t operand = opcode & 0x00FF;
    log_debug("[3XKK] - IF conditional (V[%d] == %d)", x, operand);
    if (cpu->V[x] == operand)
    {
        // Skip the next instruction
        cpu->programCounter += CH8_PC_STEP_SIZE;
    }
}

// 0x4XKK
void ch8_op_cond_neq(ch8_cpu *cpu, uint16_t opcode)
{
    assert(cpu != NULL);
    const uint8_t x = (opcode & 0x0F00) >> 8;
    const uint8_t operand = opcode & 0x00FF;
    log_debug("[4XKK] - IF conditional (V[%d] != %d)", x, operand);
    if (cpu->V[x] != operand)
    {
        // Skip the next instruction
        cpu->programCounter += CH8_PC_STEP_SIZE;
    }
}

// 0x5XY0
void ch8_op_cond_vx_eq_vy(ch8_cpu *cpu, uint16_t opcode)
{
    assert(cpu != NULL);
    const uint8_t x = (opcode & 0x0F00) >> 8;
    const uint8_t y = (opcode & 0x00F0) >> 4;
    log_debug("[5XY0] - IF conditional (V[%d] == V[%d])\n", x, y);
    if (cpu->V[x] == cpu->V[y])
    {
        // Skip the next instruction
        cpu->programCounter += CH8_PC_STEP_SIZE;
    }
}

// 0x6XKK
void ch8_op_const_set(ch8_cpu *cpu, uint16_t opcode)
{
    assert(cpu != NULL);
    const uint8_t x = (opcode & 0x0F00) >> 8;
    const uint8_t value = opcode & 0x00FF;
    log_debug("[6XKK] - SET register V[%d] = %d\n", x, value);
    cpu->V[x] = value;
}

// 0x7XKK
void ch8_op_const_add(ch8_cpu *cpu, uint16_t opcode)
{
    assert(cpu != NULL);
    const uint8_t x = (opcode & 0x0F00) >> 8;
    const uint8_t operand = opcode & 0x00FF;
    log_debug("[7XKK] - ADD %d to register V[%d]\n", operand, x);
    cpu->V[x] += operand;
}

// 0x8XY0
void ch8_op_assign(ch8_cpu *cpu, uint16_t opcode)
{
    assert(cpu != NULL);

    const uint8_t x = (opcode & 0x0F00) >> 8;
    const uint8_t y = (opcode & 0x00F0) >> 4;

    log_debug("[8XY0] - SET V[%d] = V[%d]\n", x, y);
    cpu->V[x] = cpu->V[y];
}

// 0x8XY1
void ch8_op_or(ch8_cpu *cpu, uint16_t opcode)
{
    assert(cpu != NULL);

    const uint8_t x = (opcode & 0x0F00) >> 8;
    const uint8_t y = (opcode & 0x00F0) >> 4;

    log_debug("[8XY1] - OR V[%d] = (V[%d] | V[%d])\n", x, x, y);
    cpu->V[x] |= cpu->V[y];
}

// 0x8XY2
void ch8_op_and(ch8_cpu *cpu, uint16_t opcode)
{
    assert(cpu != NULL);

    const uint8_t x = (opcode & 0x0F00) >> 8;
    const uint8_t y = (opcode & 0x00F0) >> 4;

    log_debug("[8XY2] - AND V[%d] = (V[%d] & V[%d])\n", x, x, y);
    cpu->V[x] &= cpu->V[y];
}

// 0x8XY3
void ch8_op_xor(ch8_cpu *cpu, uint16_t opcode)
{
    assert(cpu != NULL);

    const uint8_t x = (opcode & 0x0F00) >> 8;
    const uint8_t y = (opcode & 0x00F0) >> 4;

    log_debug("[8XY3] - XOR V[%d] = (V[%d] ^ V[%d])\n", x, x, y);
    cpu->V[x] ^= cpu->V[y];
}

// 0x8XY4
void ch8_op_add_vx_to_vy(ch8_cpu *cpu, uint16_t opcode)
{
    assert(cpu != NULL);

    const uint8_t x = (opcode & 0x0F00) >> 8;
    const uint8_t y = (opcode & 0x00F0) >> 4;

    const uint16_t sum = cpu->V[x] + cpu->V[y];

    //set carry flag
    cpu->V[0xF] = 0;
    if (sum > UINT8_MAX) {
        cpu->V[0xF] = 1;
    }

    cpu->V[x] = (uint8_t)sum;

    log_debug("[8XY4] - ADD V[%d] += V[%d] : V[0xF] = %d", x, y, cpu->V[0xF]);
}

// 0x8XY5
void ch8_op_sub_vy_from_vx(ch8_cpu *cpu, uint16_t opcode)
{
    assert(cpu != NULL);

    const uint8_t x = (opcode & 0x0F00) >> 8;
    const uint8_t y = (opcode & 0x00F0) >> 4;

    cpu->V[0xF] = 0;

    if (cpu->V[x] > cpu->V[y]) {
        cpu->V[0xF] = 1;
    }

    cpu->V[x] -= cpu->V[y];

    log_debug("[8XY5] - SUB V[%d] -= V[%d] : V[0xF] = %d", x, y, cpu->V[0xF]);
}

// 0x8XY6
void ch8_op_bitshift_right_vx_to_vf(ch8_cpu *cpu, uint16_t opcode)
{
    assert(cpu != NULL);

    const uint8_t x = (opcode & 0x0F00) >> 8;
    cpu->V[0xF] = cpu->V[x] & 0x1;
    cpu->V[x] >>= 1;

    log_debug("[8XY6] - SHIFTR V[%d] >>= 1", x);
}

// 0x8XY7
void ch8_op_set_vx_to_vx_sub_vy(ch8_cpu *cpu, uint16_t opcode)
{
    assert(cpu != NULL);

    const uint16_t x = (opcode & 0x0F00) >> 8;
    const uint16_t y = (opcode & 0x00F0) >> 4;
    cpu->V[0xF] = cpu->V[y] > cpu->V[x] ? 1 : 0;
    cpu->V[x] = cpu->V[y] - cpu->V[x];

    log_debug("[8XY7] - SUB V[%d] = V[%d] - V[%d]", x, y, x);
}

// 0x8XYE
void ch8_op_bitshift_left_vx_to_vf(ch8_cpu *cpu, uint16_t opcode)
{
    assert(cpu != NULL);

    const uint8_t x = (opcode & 0x0F00) >> 8;
    cpu->V[0xF] = cpu->V[x] & 0x80;
    cpu->V[x] <<= 1;

    log_debug("[8XYE] - SHIFTL V[%d] <<= 1", x);
}

// 0x9XY0
void ch8_op_cond_vx_neq_vy(ch8_cpu* cpu, uint16_t opcode)
{
    assert(cpu != NULL);
    const uint8_t x = (opcode & 0x0F00) >> 8;
    const uint8_t y = (opcode & 0x00F0) >> 4;
    log_debug("[9XY0] - IF conditional (V[%d] != V[%d])\n", x, y);
    if (cpu->V[x] != cpu->V[y])
    {
        // Skip the next instruction
        cpu->programCounter += CH8_PC_STEP_SIZE;
    }
}

// 0xANNN
void ch8_op_set_addr(ch8_cpu *cpu, uint16_t opcode)
{
    assert(cpu != NULL);
    const uint16_t addr = opcode & 0x0FFF;
    log_debug("[ANNN] - SET I = %d\n", addr);
    cpu->index = addr;
}

// 0xBNNN
void ch8_jump_to_addr_plus_v0(ch8_cpu *cpu, uint16_t opcode)
{
    assert(cpu != NULL);
    const uint16_t addr = opcode & 0x0FFF;
    log_debug("JUMP to %d + V[0]\n", addr);
    cpu->programCounter = addr + cpu->V[0];
}

// 0xCNNN
void ch8_op_bitwise_rand(ch8_cpu *cpu, uint16_t opcode)
{
    assert(cpu != NULL);
    const uint8_t x = (opcode & 0x0F00) >> 8;
    const uint8_t nn = (opcode & 0x00FF);
    log_debug("RAND V[%d] = rand() & %d\n", x, nn);
    cpu->V[x] = (rand() % 256) & nn;
}

// 0xDXYN
void ch8_op_draw_sprite(ch8_cpu *cpu, uint16_t opcode)
{
    assert(cpu != NULL);

    const uint8_t x = (opcode & 0x0F00) >> 8;
    const uint8_t y = (opcode & 0x00F0) >> 4;
    const uint8_t n = opcode & 0x000F;

    int startX = cpu->V[x];
    int startY = cpu->V[y];

    if (startX >= CH8_DISPLAY_WIDTH) {
        startX = startX % CH8_DISPLAY_WIDTH;
    }
    if (startY >= CH8_DISPLAY_HEIGHT) {
        startY = startY % CH8_DISPLAY_HEIGHT;
    }

    int endX = ch8_min(startX + 8, CH8_DISPLAY_WIDTH);
    int endY = ch8_min(startY + n, CH8_DISPLAY_HEIGHT);

    cpu->V[0xF] = 0;

    for (int y = startY; y < endY; y++) {
        uint8_t spriteByte = cpu->memory[cpu->index + (y - startY)];
        for (int x = startX; x < endX; x++) {
            // NOTE: spritePixel and screenPixel are 0 or non-zero
            // not 0 or 1 !!!
            int spritePixel = spriteByte & (0x80 >> (x - startX));
            int screenPixel = ch8_getPixel(cpu, x, y);

            if(spritePixel) {
                if(screenPixel) {
                 cpu->V[0xF] = 1;
                }

                ch8_setPixel(cpu, x, y, screenPixel == 0 ? true : false);
            }
        }
    }

    ch8_displayWriteFb(cpu);

    log_debug("[DXYN] - Draw X: %d, Y: %d, N: %d", cpu->V[x], cpu->V[y], n);
}

// 0xEX9E
void ch8_op_keyop_eq(ch8_cpu *cpu, uint16_t opcode)
{
    assert(cpu != NULL);
    const ch8_key key = (ch8_key) ((opcode & 0x0F00) >> 8);
    log_debug("KEY check if keypad[%d] is down\n", key);
    if (ch8_isKeyDown(cpu, key)) {
        cpu->programCounter += CH8_PC_STEP_SIZE;
    }
}

// 0xEXA1
void ch8_op_keyop_neq(ch8_cpu *cpu, uint16_t opcode)
{
    assert(cpu != NULL);
    const ch8_key key = (ch8_key) ((opcode & 0x0F00) >> 8);
    log_debug("KEY check if keypad[%d] is up\n", key);
    if (ch8_isKeyUp(cpu, key)) {
        cpu->programCounter += CH8_PC_STEP_SIZE;
    }
}

// 0xFX07
void ch8_op_set_vx_to_delay_timer(ch8_cpu *cpu, uint16_t opcode)
{
    assert(cpu != NULL);
    const uint16_t x = (opcode & 0x0F00) >> 8;
    cpu->V[x] = cpu->delayTimer;
    log_debug("TIMER set V[%d] = delay timer val %d", x, cpu->delayTimer);
}

// 0xFX0A
void ch8_op_await_keypress(ch8_cpu *cpu, uint16_t opcode)
{
    assert(cpu != NULL);
    log_debug("[FX0A] - Await keypress...");

    const uint16_t x = (opcode & 0x0F00) >> 8;

    ch8_key key;
    if (ch8_awaitKeyPress(cpu, &key) != 0) {
        log_error("Error awaiting keypress");
        return;
    }

    if (key != KEY_UNKNOWN) {
        cpu->V[x] = (uint8_t) key;
    }
}

// 0xFX15
void ch8_op_set_delay_timer_to_vx(ch8_cpu *cpu, uint16_t opcode)
{
    assert(cpu != NULL);

    const uint16_t x = (opcode & 0x0F00) >> 8;
    cpu->delayTimer = cpu->V[x];

    log_debug("TIMER set delay timer to V[%d] (%d)", x, cpu->delayTimer);
}

// 0xFX18
void ch8_op_set_sound_timer_to_vx(ch8_cpu *cpu, uint16_t opcode)
{
    assert(cpu != NULL);

    const uint16_t x = (opcode & 0x0F00) >> 8;
    cpu->soundTimer = cpu->V[x];

    log_debug("SOUND set sound timer to V[%d] (%d)", x, cpu->soundTimer);
}

// 0xFX1E
void ch8_op_add_vx_to_I(ch8_cpu *cpu, uint16_t opcode)
{
    assert(cpu != NULL);
    const uint16_t x = (opcode & 0x0F00) >> 8;
    cpu->index += cpu->V[x];
    log_debug("[FX1E] - ADD I += V[%d]", x);
}

// 0xFX29
void ch8_op_set_I_to_sprite_addr(ch8_cpu *cpu, uint16_t opcode)
{
    assert(cpu != NULL);
    const uint8_t x = (opcode & 0x0F00) >> 8;
    cpu->index = cpu->V[x] * 5;
    log_debug("[FX29] - SET I = V[%d] * 5 (sprite_addr)", x);
}

// 0xFX33
void ch8_op_store_bcd_of_vx(ch8_cpu *cpu, uint16_t opcode)
{
    assert(cpu != NULL);

    const uint8_t x = (opcode & 0x0F00) >> 8;

    cpu->memory[cpu->index] = (uint8_t)cpu->V[x] / 100;
    cpu->memory[cpu->index + 1] = (uint8_t)(cpu->V[x] % 100) / 10;
    cpu->memory[cpu->index + 2] = (uint8_t)cpu->V[x] % 10;

    log_debug("[FX33] - BCD store V[%d] (%d)", x, cpu->V[x]);
}

// 0xFX55
void ch8_op_store_v0_to_vx(ch8_cpu *cpu, uint16_t opcode)
{
    assert(cpu != NULL);
    const uint8_t x = (opcode & 0x0F00) >> 8;

    for (int i = 0; i <= x; i++) {
        cpu->memory[cpu->index + i] = cpu->V[i];
    }

    //cpu->index_register += x + 1;

    log_debug("[FX55] - STORE V[0]..V[%d] in memory", x);
}

// 0xFX65
void ch8_op_fill_v0_to_vx(ch8_cpu *cpu, uint16_t opcode)
{
    assert(cpu != NULL);
    const uint8_t x = (opcode & 0x0F00) >> 8;

    for (int i = 0; i <= x; i++) {
        cpu->V[i] = cpu->memory[cpu->index + i];
    }

    //cpu->index_register += x + 1;

    log_debug("[FX55] - FILL memory bytes into V[0]..V[%d]", x);
}
