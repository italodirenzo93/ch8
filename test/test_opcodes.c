#include <string.h>

#include "vendor/unity.h"
#include "opcodes.h"
#include "display.h"
#include "ch8_cpu.h"

// STUBS
void display_clear()
{
    TEST_PASS_MESSAGE("display_clear called");
}

void draw_sprite(ch8_cpu *cpu, uint8_t x, uint8_t y, uint8_t h)
{
    TEST_PASS_MESSAGE("draw_sprite called");
}

// TESTS
ch8_cpu chip8;

void setUp()
{
    memset(&chip8, 0, sizeof(ch8_cpu));
}

void tearDown()
{
}

static void jumpto_sets_pc_to_address()
{
    uint16_t opcode = 0x1323;
    uint16_t addr = opcode & 0x0FFF;
    ch8_op_jumpto(&chip8, opcode);
    TEST_ASSERT_EQUAL(addr, chip8.PC);
}

static void cond_eq_skips_next_instruction_if_equal()
{
    uint16_t opcode = 0x2305;
    uint8_t vx = (opcode & 0x0F00) >> 8;

    chip8.PC = 144;
    chip8.V[vx] = 5;

    ch8_op_cond_eq(&chip8, opcode);
    TEST_ASSERT_EQUAL(146, chip8.PC);
}

static void cond_eq_does_not_skip_next_instruction_if_unequal()
{
    uint16_t opcode = 0x23EE;
    uint8_t vx = (opcode & 0x0F00) >> 8;

    chip8.PC = 144;
    chip8.V[vx] = 5;

    ch8_op_cond_eq(&chip8, opcode);
    TEST_ASSERT_EQUAL(144, chip8.PC);
}

static void cond_neq_skips_next_instruction_if_nequal()
{
    uint16_t opcode = 0x23FF;
    uint8_t vx = (opcode & 0x0F00) >> 8;

    chip8.PC = 144;
    chip8.V[vx] = 27;

    ch8_op_cond_neq(&chip8, opcode);
    TEST_ASSERT_EQUAL(146, chip8.PC);
}

static void cond_neq_does_not_skip_next_instruction_if_equal()
{
    uint16_t opcode = 0x23FF;
    uint8_t vx = (opcode & 0x0F00) >> 8;

    chip8.PC = 144;
    chip8.V[vx] = 255;

    ch8_op_cond_neq(&chip8, opcode);
    TEST_ASSERT_EQUAL(144, chip8.PC);
}

static void cond_vx_eq_vy_skips_next_instruction_if_equal()
{
    uint16_t opcode = 0x23FF;
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t vy = (opcode & 0x00F0) >> 4;

    chip8.PC = 144;
    chip8.V[vx] = 255;
    chip8.V[vy] = 255;

    ch8_op_cond_vx_eq_vy(&chip8, opcode);
    TEST_ASSERT_EQUAL(146, chip8.PC);
}

static void cond_vx_eq_vy_does_not_skip_next_instruction_if_unequal()
{
    uint16_t opcode = 0x23FF;
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t vy = (opcode & 0x00F0) >> 4;

    chip8.PC = 144;
    chip8.V[vx] = 255;
    chip8.V[vy] = 254;

    ch8_op_cond_vx_eq_vy(&chip8, opcode);
    TEST_ASSERT_EQUAL(144, chip8.PC);
}

static void const_set_sets_vx_to_value()
{
    uint16_t opcode = 0x670C;
    ch8_op_const_set(&chip8, opcode);
    TEST_ASSERT_EQUAL(12, chip8.V[7]);
}

static void const_add_adds_value_to_vx()
{
    uint16_t opcode = 0x780C;
    chip8.V[8] = 4;
    ch8_op_const_add(&chip8, opcode);
    TEST_ASSERT_EQUAL(16, chip8.V[8]);
}

static void assign_vx_to_vy()
{
    chip8.V[3] = 45;
    chip8.V[6] = 0;
    ch8_op_assign(&chip8, 0x8630);
    TEST_ASSERT_EQUAL(45, chip8.V[6]);
}

static void bitwise_or()
{
    chip8.V[8] = 1;
    chip8.V[1] = 2;
    ch8_op_or(&chip8, 0x8811);
    TEST_ASSERT_EQUAL(3, chip8.V[8]);
}

static void bitwise_and()
{
    chip8.V[8] = 0x11;
    chip8.V[1] = 0xF3;
    ch8_op_and(&chip8, 0x8812);
    TEST_ASSERT_EQUAL(17, chip8.V[8]);
}

static void bitwise_xor()
{
    chip8.V[2] = 0x34;
    chip8.V[3] = 0x32;
    ch8_op_xor(&chip8, 0x8233);
    TEST_ASSERT_EQUAL(6, chip8.V[2]);
}

static void set_i_to_address()
{
    ch8_op_set_addr(&chip8, 0xA3FF);
    TEST_ASSERT_EQUAL(0x03FF, chip8.I);
}

static void jump_to_addr_plus_v0()
{
    chip8.V[0] = 0x04;
    ch8_jump_to_addr_plus_v0(&chip8, 0xB199);
    TEST_ASSERT_EQUAL(413, chip8.PC);
}

// 0xEX9E
static void key_down_skip_next_instruction()
{
    chip8.keypad[8] = CH8_KEYDOWN;
    chip8.PC = 5;
    ch8_op_keyop_eq(&chip8, 0xE800);
    TEST_ASSERT_EQUAL(7, chip8.PC);
}

static void key_down_does_not_skip_next_instruction()
{
    chip8.keypad[8] = CH8_KEYDOWN;
    chip8.keypad[4] = CH8_KEYUP;
    chip8.PC = 5;
    ch8_op_keyop_eq(&chip8, 0xE400);
    TEST_ASSERT_EQUAL(5, chip8.PC);
}

// 0xEXA1
static void key_up_skip_next_instruction()
{
    chip8.keypad[8] = CH8_KEYDOWN;
    chip8.PC = 5;
    ch8_op_keyop_neq(&chip8, 0xE800);
    TEST_ASSERT_EQUAL(5, chip8.PC);
}

static void key_up_does_not_skip_next_instruction()
{
    chip8.keypad[9] = CH8_KEYDOWN;
    chip8.keypad[4] = CH8_KEYUP;
    chip8.PC = 5;
    ch8_op_keyop_neq(&chip8, 0xE400);
    TEST_ASSERT_EQUAL(7, chip8.PC);
}

int main()
{
    UnityBegin("test/test_opcodes.c");

    // 1NNN
    RUN_TEST(jumpto_sets_pc_to_address);

    // 3XNN
    RUN_TEST(cond_eq_skips_next_instruction_if_equal);
    RUN_TEST(cond_eq_does_not_skip_next_instruction_if_unequal);

    // 4XNN
    RUN_TEST(cond_neq_skips_next_instruction_if_nequal);
    RUN_TEST(cond_neq_does_not_skip_next_instruction_if_equal);

    // 5XY0
    RUN_TEST(cond_vx_eq_vy_skips_next_instruction_if_equal);
    RUN_TEST(cond_vx_eq_vy_does_not_skip_next_instruction_if_unequal);

    // 6XNN
    RUN_TEST(const_set_sets_vx_to_value);

    // 7XNN
    RUN_TEST(const_add_adds_value_to_vx);

    // 8XY0
    RUN_TEST(assign_vx_to_vy);
    RUN_TEST(bitwise_or);
    RUN_TEST(bitwise_and);
    RUN_TEST(bitwise_xor);

    // 0xA000
    RUN_TEST(set_i_to_address);

    // 0xB000
    RUN_TEST(jump_to_addr_plus_v0);

    // 0xC000

    // 0xD000

    // 0xE000
    RUN_TEST(key_down_skip_next_instruction);
    RUN_TEST(key_down_does_not_skip_next_instruction);
    RUN_TEST(key_up_skip_next_instruction);
    RUN_TEST(key_up_does_not_skip_next_instruction);

    return UnityEnd();
}
