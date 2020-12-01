#include "vendor/unity.h"
#include "opcodes.h"
#include "display.h"
#include "ch8_cpu.h"
#include "keyboard.h"

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
    ch8_reset(&chip8);
}

void tearDown()
{
}

static void return_from_subroutine_sets_pc_to_last_stack_address(void)
{
    const uint16_t addr = 0xE21F;
    chip8.stack[7] = addr;
    chip8.stack_pointer = 8;
    
    ch8_op_return(&chip8);
    
    TEST_ASSERT_EQUAL(addr, chip8.program_counter);
    TEST_ASSERT_EQUAL(7, chip8.stack_pointer);
}

static void jumpto_sets_pc_to_address(void)
{
    const uint16_t opcode = 0x1323;
    const uint16_t addr = opcode & 0x0FFF;
    ch8_op_jumpto(&chip8, opcode);
    TEST_ASSERT_EQUAL(addr, chip8.program_counter);
}

static void cond_eq_skips_next_instruction_if_equal(void)
{
    const uint16_t opcode = 0x2305;
    const uint8_t vx = (opcode & 0x0F00) >> 8;

    chip8.program_counter = 144;
    chip8.V[vx] = 5;

    ch8_op_cond_eq(&chip8, opcode);
    TEST_ASSERT_EQUAL(146, chip8.program_counter);
}

static void cond_eq_does_not_skip_next_instruction_if_unequal(void)
{
    const uint16_t opcode = 0x23EE;
    const uint8_t vx = (opcode & 0x0F00) >> 8;

    chip8.program_counter = 144;
    chip8.V[vx] = 5;

    ch8_op_cond_eq(&chip8, opcode);
    TEST_ASSERT_EQUAL(144, chip8.program_counter);
}

static void cond_neq_skips_next_instruction_if_nequal(void)
{
    const uint16_t opcode = 0x23FF;
    const uint8_t vx = (opcode & 0x0F00) >> 8;

    chip8.program_counter = 144;
    chip8.V[vx] = 27;

    ch8_op_cond_neq(&chip8, opcode);
    TEST_ASSERT_EQUAL(146, chip8.program_counter);
}

static void cond_neq_does_not_skip_next_instruction_if_equal(void)
{
    const uint16_t opcode = 0x23FF;
    const uint8_t vx = (opcode & 0x0F00) >> 8;

    chip8.program_counter = 144;
    chip8.V[vx] = 255;

    ch8_op_cond_neq(&chip8, opcode);
    TEST_ASSERT_EQUAL(144, chip8.program_counter);
}

static void cond_vx_eq_vy_skips_next_instruction_if_equal(void)
{
    const uint16_t opcode = 0x23FF;
    const uint8_t vx = (opcode & 0x0F00) >> 8;
    const uint8_t vy = (opcode & 0x00F0) >> 4;

    chip8.program_counter = 144;
    chip8.V[vx] = 255;
    chip8.V[vy] = 255;

    ch8_op_cond_vx_eq_vy(&chip8, opcode);
    TEST_ASSERT_EQUAL(146, chip8.program_counter);
}

static void cond_vx_eq_vy_does_not_skip_next_instruction_if_unequal(void)
{
    const uint16_t opcode = 0x23FF;
    const uint8_t vx = (opcode & 0x0F00) >> 8;
    const uint8_t vy = (opcode & 0x00F0) >> 4;

    chip8.program_counter = 144;
    chip8.V[vx] = 255;
    chip8.V[vy] = 254;

    ch8_op_cond_vx_eq_vy(&chip8, opcode);
    TEST_ASSERT_EQUAL(144, chip8.program_counter);
}

static void const_set_sets_vx_to_value(void)
{
    const uint16_t opcode = 0x670C;
    ch8_op_const_set(&chip8, opcode);
    TEST_ASSERT_EQUAL(12, chip8.V[7]);
}

static void const_add_adds_value_to_vx(void)
{
    const uint16_t opcode = 0x780C;
    chip8.V[8] = 4;
    ch8_op_const_add(&chip8, opcode);
    TEST_ASSERT_EQUAL(16, chip8.V[8]);
}

static void assign_vx_to_vy(void)
{
    chip8.V[3] = 45;
    chip8.V[6] = 0;
    ch8_op_assign(&chip8, 0x8630);
    TEST_ASSERT_EQUAL(45, chip8.V[6]);
}

static void bitwise_or(void)
{
    chip8.V[8] = 1;
    chip8.V[1] = 2;
    ch8_op_or(&chip8, 0x8811);
    TEST_ASSERT_EQUAL(3, chip8.V[8]);
}

static void bitwise_and(void)
{
    chip8.V[8] = 0x11;
    chip8.V[1] = 0xF3;
    ch8_op_and(&chip8, 0x8812);
    TEST_ASSERT_EQUAL(17, chip8.V[8]);
}

static void bitwise_xor(void)
{
    chip8.V[2] = 0x34;
    chip8.V[3] = 0x32;
    ch8_op_xor(&chip8, 0x8233);
    TEST_ASSERT_EQUAL(6, chip8.V[2]);
}

// 0x8XY4
static void add_vx_to_vy_sets_carry_to_1_if_overflow(void)
{
    chip8.V[0] = 150;
    chip8.V[1] = 150;
    chip8.V[0xF] = 0;

    ch8_op_add_vx_to_vy(&chip8, 0x8014);

    TEST_ASSERT_EQUAL(44, chip8.V[0]);  // should overflow
    TEST_ASSERT_EQUAL(1, chip8.V[0xF]);
}

static void add_vx_to_vy_sets_carry_to_0_if_no_overflow(void)
{
    chip8.V[0] = 75;
    chip8.V[1] = 25;
    chip8.V[0xF] = 0;

    ch8_op_add_vx_to_vy(&chip8, 0x8014);

    TEST_ASSERT_EQUAL(100, chip8.V[0]); // no overflow
    TEST_ASSERT_EQUAL(0, chip8.V[0xF]);
}

// 0x8XY5
static void sub_vy_from_vx_sets_borrow_to_0_if_underflow(void)
{
    chip8.V[0] = 6;
    chip8.V[1] = 7;
    chip8.V[0xF] = 0;

    ch8_op_sub_vy_from_vx(&chip8, 0x8015);

    TEST_ASSERT_EQUAL(255, chip8.V[0]); // should underflow
    TEST_ASSERT_EQUAL(0, chip8.V[0xF]);
}

static void sub_vy_from_vx_sets_borrow_to_1_if_no_underflow(void)
{
    chip8.V[0] = 7;
    chip8.V[1] = 3;
    chip8.V[0xF] = 0;

    ch8_op_sub_vy_from_vx(&chip8, 0x8015);

    TEST_ASSERT_EQUAL(4, chip8.V[0]);
    TEST_ASSERT_EQUAL(1, chip8.V[0xF]);
}

static void set_i_to_address(void)
{
    ch8_op_set_addr(&chip8, 0xA3FF);
    TEST_ASSERT_EQUAL(0x03FF, chip8.index_register);
}

static void jump_to_addr_plus_v0(void)
{
    chip8.V[0] = 0x04;
    ch8_jump_to_addr_plus_v0(&chip8, 0xB199);
    TEST_ASSERT_EQUAL(413, chip8.program_counter);
}

// 0xEX9E
static void key_down_skip_next_instruction(void)
{
    chip8.keypad[8] = true;
    chip8.program_counter = 5;
    ch8_op_keyop_eq(&chip8, 0xE800);
    TEST_ASSERT_EQUAL(7, chip8.program_counter);
}

static void key_down_does_not_skip_next_instruction(void)
{
    chip8.keypad[8] = true;
    chip8.keypad[4] = false;
    chip8.program_counter = 5;
    ch8_op_keyop_eq(&chip8, 0xE400);
    TEST_ASSERT_EQUAL(5, chip8.program_counter);
}

// 0xEXA1
static void key_up_skip_next_instruction(void)
{
    chip8.keypad[8] = true;
    chip8.program_counter = 5;
    ch8_op_keyop_neq(&chip8, 0xE800);
    TEST_ASSERT_EQUAL(5, chip8.program_counter);
}

static void key_up_does_not_skip_next_instruction(void)
{
    chip8.keypad[9] = true;
    chip8.keypad[4] = false;
    chip8.program_counter = 5;
    ch8_op_keyop_neq(&chip8, 0xE400);
    TEST_ASSERT_EQUAL(7, chip8.program_counter);
}

// 0xFX33
static void test_store_bcd_of_vx_at_i(void)
{
    chip8.V[3] = 255;
    chip8.index_register = 764;

    ch8_op_store_bcd_of_vx(&chip8, 0xF333);

    TEST_ASSERT_EQUAL(2, chip8.memory[chip8.index_register]);
    TEST_ASSERT_EQUAL(5, chip8.memory[chip8.index_register + 1]);
    TEST_ASSERT_EQUAL(5, chip8.memory[chip8.index_register + 2]);
}

// 0xFX55
static void test_store_V0_to_Vx(void)
{
    const uint16_t start_addr = 2078; /* random memory offset */

    chip8.index_register = start_addr;
    chip8.V[0] = 5;
    chip8.V[1] = 4;
    chip8.V[2] = 22;
    chip8.V[3] = 144;

    ch8_op_store_v0_to_vx(&chip8, 0xF355);

    TEST_ASSERT_EQUAL(5, chip8.memory[start_addr]);
    TEST_ASSERT_EQUAL(4, chip8.memory[start_addr + 1]);
    TEST_ASSERT_EQUAL(22, chip8.memory[start_addr + 2]);
    TEST_ASSERT_EQUAL(144, chip8.memory[start_addr + 3]);

    //TEST_ASSERT_EQUAL(2083, chip8.index_register); /* I = I + x + 1 */
}

// 0xFX65
static void test_fill_V0_to_Vx(void)
{
    const uint16_t start_addr = 2078; /* random memory offset */

    chip8.index_register = start_addr;
    chip8.memory[start_addr] = 5;
    chip8.memory[start_addr + 1] = 4;
    chip8.memory[start_addr + 2] = 22;
    chip8.memory[start_addr + 3] = 144;

    ch8_op_fill_v0_to_vx(&chip8, 0xF365);

    TEST_ASSERT_EQUAL(5, chip8.V[0]);
    TEST_ASSERT_EQUAL(4, chip8.V[1]);
    TEST_ASSERT_EQUAL(22, chip8.V[2]);
    TEST_ASSERT_EQUAL(144, chip8.V[3]);

    //TEST_ASSERT_EQUAL(2083, chip8.index_register); /* I = I + x + 1 */
}

int main()
{
    UnityBegin("test/test_opcodes.c");

    // 1NNN
    RUN_TEST(return_from_subroutine_sets_pc_to_last_stack_address);
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
    RUN_TEST(add_vx_to_vy_sets_carry_to_1_if_overflow);
    RUN_TEST(add_vx_to_vy_sets_carry_to_0_if_no_overflow);
    RUN_TEST(sub_vy_from_vx_sets_borrow_to_0_if_underflow);
    RUN_TEST(sub_vy_from_vx_sets_borrow_to_1_if_no_underflow);

    // 0xA000
    RUN_TEST(set_i_to_address);

    // 0xB000
    RUN_TEST(jump_to_addr_plus_v0);

    // 0xC000

    // 0xD000

    // 0xE000
    //RUN_TEST(key_down_skip_next_instruction);
    //RUN_TEST(key_down_does_not_skip_next_instruction);
    //RUN_TEST(key_up_skip_next_instruction);
    //RUN_TEST(key_up_does_not_skip_next_instruction);

    // 0xF000
    RUN_TEST(test_store_bcd_of_vx_at_i);
    RUN_TEST(test_store_V0_to_Vx);
    RUN_TEST(test_fill_V0_to_Vx);

    return UnityEnd();
}
