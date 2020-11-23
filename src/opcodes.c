#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <SDL.h>

#include "opcodes.h"
#include "display.h"
#include "ch8_cpu.h"
#include "log.h"
#include "input.h"

#define TIMER_INTERVAL 17

static int _delay_timer_thread_fn(void* data)
{
    ch8_cpu* cpu = (ch8_cpu*)data;
    if (SDL_AtomicGet(&cpu->delayTimer) <= 0) {
        return EXIT_FAILURE;
    }
    
    log_debug("Starting delay timer thread");

    while (cpu->running && !SDL_AtomicDecRef(&cpu->delayTimer)) {
        SDL_Delay(TIMER_INTERVAL);
    }
    
    log_debug("Exiting delay timer thread");

    return EXIT_SUCCESS;
}

static int _sound_timer_thread_fn(void* data)
{
    ch8_cpu* cpu = (ch8_cpu*)data;
    if (SDL_AtomicGet(&cpu->soundTimer) <= 0) {
        return EXIT_FAILURE;
    }
    
    log_debug("Starting sound timer thread");

    while (cpu->running && !SDL_AtomicDecRef(&cpu->soundTimer)) {
        SDL_Delay(TIMER_INTERVAL);
        // pump sound data
    }

    // stop pumping sound data
    
    log_debug("Exiting sound timer thread");

    return EXIT_SUCCESS;
}

void ch8_op_return(ch8_cpu *cpu)
{
    assert(cpu != NULL);
    log_debug("Return from subroutine...\n");
    cpu->PC = CH8_PROGRAM_START_OFFSET;
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
    for (i = 0; i < CH8_DISPLAY_SIZE; i++) {
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
        cpu->PC += PC_STEP_SIZE;
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
        cpu->PC += PC_STEP_SIZE;
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
        cpu->PC += PC_STEP_SIZE;
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
        cpu->PC += PC_STEP_SIZE;
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
    uint8_t startIdx = vy * DISPLAY_WIDTH + vx;
    
    uint8_t destX = vx + 8;
    uint8_t destY = vy + n;
    uint8_t destIdx = destY * DISPLAY_WIDTH + destX;
    
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
        cpu->PC += PC_STEP_SIZE;
    }
}

void ch8_op_keyop_neq(ch8_cpu *cpu, uint16_t opcode)
{
    assert(cpu != NULL);
    input_key key = (input_key) ((opcode & 0x0F00) >> 8);
    log_debug("KEY check if keypad[%d] is up\n", key);
    if (is_key_up(cpu, key)) {
        cpu->PC += PC_STEP_SIZE;
    }
}

// 0xFX07
void ch8_op_set_vx_to_delay_timer(ch8_cpu *cpu, uint16_t opcode)
{
    uint16_t vx = (opcode & 0x0F00) >> 8;
    int timerValue = SDL_AtomicGet(&cpu->delayTimer);
    cpu->V[vx] = timerValue > UINT8_MAX ? UINT8_MAX : timerValue < 0 ? 0 : (uint8_t) timerValue;
    log_debug("TIMER set V[%d] = delay timer val %d", vx, timerValue);
}

// 0xFX15
void ch8_op_set_delay_timer_to_vx(ch8_cpu *cpu, uint16_t opcode)
{
    uint16_t vx = (opcode & 0x0F00) >> 8;
    uint8_t timerVal = cpu->V[vx];
    int prev = SDL_AtomicSet(&cpu->delayTimer, timerVal);

    // If the timer was previously set to 0, then a thread isn't running and we need to start a new one
    if (prev == 0) {
        SDL_Thread* thread = SDL_CreateThread(_delay_timer_thread_fn, "DelayTimer", cpu);
        SDL_DetachThread(thread);
    }

    log_debug("TIMER set delay timer to V[%d] (%d)", vx, timerVal);
}

// 0xFX18
void ch8_op_set_sound_timer_to_vx(ch8_cpu *cpu, uint16_t opcode)
{
    uint16_t vx = (opcode & 0x0F00) >> 8;
    uint8_t timerVal = cpu->V[vx];
    int prev = SDL_AtomicSet(&cpu->soundTimer, timerVal);

    // If the timer was previously set to 0, then a thread isn't running and we need to start a new one
    if (prev == 0) {
        SDL_Thread* thread = SDL_CreateThread(_sound_timer_thread_fn, "SoundTimer", cpu);
        SDL_DetachThread(thread);
    }

    log_debug("SOUND set sound timer to V[%d] (%d)", vx, timerVal);
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
