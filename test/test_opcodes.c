#include "vendor/unity.h"

#include "../src/ch8_opcodes.h"
#include "../src/ch8_util.h"

ch8_cpu chip8;

void setUp()
{
    ch8_reset(&chip8);
}

void tearDown()
{
}

// 00E0
static void test_00E0_ClearScreen_SetsAllPixelsTo0(void)
{
    // arrange
    for (int i = 0; i < CH8_DISPLAY_SIZE; i++) {
        chip8.framebuffer[i] = ch8_randU8();
    }

    // act
    ch8_op_ClearDisplay(&chip8);

    // assert
    TEST_ASSERT_EACH_EQUAL_UINT8(0, chip8.framebuffer, CH8_DISPLAY_SIZE);
}

// 00EE
static void test_00EE_ReturnFromSubroutine_SetsProgramCounterToStackTop(void)
{
    // arrange
    u16 addr = 0x0123;

    chip8.stack[0] = ch8_randU16();
    chip8.stack[1] = ch8_randU16();
    chip8.stack[2] = addr;
    chip8.stackPointer = 2;

    // act
    ch8_op_ReturnFromSub(&chip8);

    // assert
    TEST_ASSERT_EQUAL(addr, chip8.programCounter);
    TEST_ASSERT_EQUAL(1, chip8.stackPointer);
}

// 1NNN
static void test_1NNN_Jump_SetsProgramCounterToAddress(void)
{
    // arrange
    u16 opcode = 0x1323;
    u16 addr = 0x323;

    // act
    ch8_op_JumpTo(&chip8, opcode);

    // assert
    TEST_ASSERT_EQUAL(addr, chip8.programCounter);
}

// 2NNN
static void test_2NNN_CallSub_SetsProgramCounterToAddress(void)
{
    // arrange
    u16 opcode = 0x2333;
    u16 addr = 0x333;

    chip8.programCounter = 0x204;

    // act
    ch8_op_CallSub(&chip8, opcode);

    // assert
    TEST_ASSERT_EQUAL(addr, chip8.programCounter);
}

static void test_2NNN_CallSub_PushesTheCurrentAddressOntoStack(void)
{
    // arrange
    chip8.stack[0] = ch8_randU16();
    chip8.stack[1] = ch8_randU16();
    chip8.stack[2] = ch8_randU16();

    chip8.stackPointer = 2;
    chip8.programCounter = 0x222;

    // act
    ch8_op_CallSub(&chip8, 0x2123);

    // assert
    TEST_ASSERT_EQUAL(3, chip8.stackPointer);
    TEST_ASSERT_EQUAL(0X222, chip8.stack[chip8.stackPointer]);
}

// 3XNN
static void test_3XNN_SkipEquals_SkipsTheNextInstructionIfEqualNN(void)
{
    // arrange
    u16 opcode = 0x2305;
    u8 vx = 0x3;

    chip8.programCounter = 0x235;
    chip8.V[vx] = 0x5;

    // act
    ch8_op_SkipEquals(&chip8, opcode);

    // assert
    // Skips the next instruction and then advances the program counter
    TEST_ASSERT_EQUAL(0x239, chip8.programCounter);
}

static void test_3XNN_SkipEquals_DoesNotSkipIfNotEqualNN(void)
{
    // arrange
    u16 opcode = 0x2305;
    u8 vx = 0x3;

    chip8.programCounter = 0x235;
    chip8.V[vx] = 0x6;

    // act
    ch8_op_SkipEquals(&chip8, opcode);

    // assert
    // Advances the program counter but does not skip the next instruction
    TEST_ASSERT_EQUAL(0x237, chip8.programCounter);
}

// 4XNN
static void test_4XNN_SkipNotEquals_SkipsTheNextInstructionIfNotEqualNN(void)
{
    // arrange
    u16 opcode = 0x2301;
    u8 vx = 0x1;

    chip8.programCounter = 0x203;
    chip8.V[vx] = 27;

    // act
    ch8_op_SkipNotEquals(&chip8, opcode);

    // assert
    TEST_ASSERT_EQUAL(0x207, chip8.programCounter);
}

static void test_4XNN_SkipNotEquals_DoesNotSkipNextInstructionIfEqualNN(void)
{
    // arrange
    u16 opcode = 0x23FF;
    u8 vx = 0x2;

    chip8.programCounter = 0x203;
    chip8.V[vx] = 3;

    // act
    ch8_op_SkipNotEquals(&chip8, opcode);

    // assert
    TEST_ASSERT_EQUAL(0x207, chip8.programCounter);
}

// 5XY0
static void test_5XYN_SkipVXEqualsVY_SkipsNextInstructionIfVXEqualsVY(void)
{
    chip8.programCounter = 144;
    chip8.V[2] = 255;
    chip8.V[3] = 255;

    ch8_op_SkipVXEqualsVY(&chip8, 0x5230);
    TEST_ASSERT_EQUAL(148, chip8.programCounter);
}

static void test_5XYN_SkipVXEqualsVY_DoesNotSkipNextInstructionIfVXDoesNotEqualVY(void)
{
    chip8.programCounter = 144;
    chip8.V[2] = 255;
    chip8.V[3] = 254;

    ch8_op_SkipVXEqualsVY(&chip8, 0x5230);
    TEST_ASSERT_EQUAL(146, chip8.programCounter);
}

// 6XNN
static void test_6XNN_Set_SetsVXToNN(void)
{
    chip8.programCounter = 212;
    ch8_op_Set(&chip8, 0X71E);
    TEST_ASSERT_EQUAL(0x1E, chip8.V[7]);
    TEST_ASSERT_EQUAL(214, chip8.programCounter);
}

// 7XNN
static void test_7XNN_Add_AddsNNToVX(void)
{
    chip8.V[8] = 4;
    chip8.programCounter = 212;
    ch8_op_Add(&chip8, 0x780C);
    TEST_ASSERT_EQUAL(16, chip8.V[8]);
    TEST_ASSERT_EQUAL(214, chip8.programCounter);
}

// 8XY0
static void test_8XY0_Assign_SetsVXToVY(void)
{
    chip8.V[3] = 45;
    chip8.V[6] = 0;
    chip8.programCounter = 212;
    ch8_op_Assign(&chip8, 0x8630);
    TEST_ASSERT_EQUAL(45, chip8.V[6]);
    TEST_ASSERT_EQUAL(214, chip8.programCounter);
}

static void test_8XY1_LogicalOr_SetsVXToVXOrVY(void)
{
    chip8.V[8] = 1;
    chip8.V[1] = 2;
    chip8.programCounter = 212;
    ch8_op_LogicalOr(&chip8, 0x8811);
    TEST_ASSERT_EQUAL(3, chip8.V[8]);
    TEST_ASSERT_EQUAL(214, chip8.programCounter);
}

// 8XY2
static void test_8XY2_LogicalAnd_SetsVXToVXAndVY(void)
{
    chip8.V[8] = 0x11;
    chip8.V[1] = 0xF3;
    chip8.programCounter = 212;
    ch8_op_LogicalAnd(&chip8, 0x8812);
    TEST_ASSERT_EQUAL(17, chip8.V[8]);
    TEST_ASSERT_EQUAL(214, chip8.programCounter);
}

// 8XY3
static void test_8XY3_LogicalXor_SetsVXToVXXorVY(void)
{
    chip8.V[2] = 0x34;
    chip8.V[3] = 0x32;
    chip8.programCounter = 212;
    ch8_op_LogicalXor(&chip8, 0x8233);
    TEST_ASSERT_EQUAL(6, chip8.V[2]);
    TEST_ASSERT_EQUAL(214, chip8.programCounter);
}

// 8XY4
static void test_8XY4_Add_SetsCarryTo1IfOverflow(void)
{
    chip8.V[0] = 150;
    chip8.V[1] = 150;
    chip8.V[0xF] = 0;

    chip8.programCounter = 212;

    ch8_op_AddAssign(&chip8, 0x8014);

    TEST_ASSERT_EQUAL(44, chip8.V[0]);  // should overflow
    TEST_ASSERT_EQUAL(1, chip8.V[0xF]);
    TEST_ASSERT_EQUAL(214, chip8.programCounter);
}

static void test_8XY4_Add_SetsCarryTo0IfNoOverflow(void)
{
    chip8.V[0] = 75;
    chip8.V[1] = 25;
    chip8.V[0xF] = 0;

    chip8.programCounter = 212;

    ch8_op_AddAssign(&chip8, 0x8014);

    TEST_ASSERT_EQUAL(100, chip8.V[0]); // no overflow
    TEST_ASSERT_EQUAL(0, chip8.V[0xF]);
    TEST_ASSERT_EQUAL(214, chip8.programCounter);
}

// 8XY5
static void test_8XY5_SubtractAssign_SetsBorrowTo0IfUnderflow(void)
{
    chip8.V[0] = 6;
    chip8.V[1] = 7;
    chip8.V[0xF] = 0;

    chip8.programCounter = 212;

    ch8_op_SubtractAssign(&chip8, 0x8015);

    TEST_ASSERT_EQUAL(255, chip8.V[0]); // should underflow
    TEST_ASSERT_EQUAL(0, chip8.V[0xF]);
    TEST_ASSERT_EQUAL(214, chip8.programCounter);
}

static void test_8XY5_SubtractAssign_SetsBorrowTo1IfNoUnderflow(void)
{
    chip8.V[0] = 7;
    chip8.V[1] = 3;
    chip8.V[0xF] = 0;

    chip8.programCounter = 212;

    ch8_op_SubtractAssign(&chip8, 0x8015);

    TEST_ASSERT_EQUAL(4, chip8.V[0]);
    TEST_ASSERT_EQUAL(1, chip8.V[0xF]);
    TEST_ASSERT_EQUAL(214, chip8.programCounter);
}

// 8XY6

// 8XY7
static void test_8XY7_SubtractAssignReverse_SetsBorrowTo0IfUnderflow(void)
{
    chip8.V[0] = 7;
    chip8.V[1] = 4;

    chip8.programCounter = 212;

    ch8_op_SubtractAssignReverse(&chip8, 0x8015);

    TEST_ASSERT_EQUAL(253, chip8.V[0]); // should underflow
    TEST_ASSERT_EQUAL(0, chip8.V[0xF]);
    TEST_ASSERT_EQUAL(214, chip8.programCounter);
}

static void test_8XY7_SubtractAssignReverse_SetsBorrowTo1IfNoUnderflow(void)
{
    chip8.V[0] = 3;
    chip8.V[1] = 7;
    chip8.V[0xF] = 0;

    chip8.programCounter = 212;

    ch8_op_SubtractAssignReverse(&chip8, 0x8015);

    TEST_ASSERT_EQUAL(4, chip8.V[0]);
    TEST_ASSERT_EQUAL(1, chip8.V[0xF]);
    TEST_ASSERT_EQUAL(214, chip8.programCounter);
}

// 8XYE

// 9XYE
static void test_9XYN_SkipVXNotEqualsVY_SkipsNextInstructionIfVXNotEqualsVY(void)
{
    chip8.programCounter = 144;
    chip8.V[2] = 255;
    chip8.V[3] = 254;

    ch8_op_SkipVXNotEqualsVY(&chip8, 0x9230);
    TEST_ASSERT_EQUAL(148, chip8.programCounter);
}

static void test_9XYN_SkipVXNotEqualsVY_DoesNotSkipNextInstructionIfVXEqualsVY(void)
{
    chip8.programCounter = 144;
    chip8.V[2] = 255;
    chip8.V[3] = 255;

    ch8_op_SkipVXNotEqualsVY(&chip8, 0x9230);
    TEST_ASSERT_EQUAL(146, chip8.programCounter);
}

// A000
static void test_ANNN_SetIndex_SetsTheIndexRegisterToNNN(void)
{
    chip8.programCounter = 212;
    ch8_op_SetIndex(&chip8, 0xA3FF);
    TEST_ASSERT_EQUAL(0x03FF, chip8.index);
    TEST_ASSERT_EQUAL(214, chip8.programCounter);
}

// BXNN
static void test_BXNN_JumpOffset_SetsPCToNNPlusVX(void)
{
    chip8.V[1] = 0x04;
    ch8_op_JumpOffset(&chip8, 0xB199);
    TEST_ASSERT_EQUAL(157, chip8.programCounter);
}

// CXNN
static void test_CXNN_BitwiseRandom_AdvancesProgramCounter(void)
{
    chip8.programCounter = 212;

    ch8_op_BitwiseRandom(&chip8, 0xC422);

    TEST_ASSERT_EQUAL(214, chip8.programCounter);
}

// DXYN
static void test_DXYN_DrawSprite_SetsTheDrawFlag(void)
{
    chip8.programCounter = 212;
    chip8.drawFlag = false;

    ch8_op_DrawSprite(&chip8, 0xD142);

    TEST_ASSERT_TRUE(chip8.drawFlag);
    TEST_ASSERT_EQUAL(214, chip8.programCounter);
}

// EX9E
static void test_EX9E_KeyDown_SkipsNextInstructionIfKeyIsDown(void)
{
    chip8.keypad[8] = true;
    chip8.programCounter = 5;

    ch8_op_KeyEquals(&chip8, 0xE89E);

    TEST_ASSERT_EQUAL(9, chip8.programCounter); // skips the next instruction on top of advancing the counter
}

static void test_EX9E_KeyDown_DoesNotSkipNextInstructionIfKeyUp(void)
{
    chip8.keypad[8] = true;
    chip8.keypad[4] = false;
    chip8.programCounter = 5;

    ch8_op_KeyEquals(&chip8, 0xE49E);

    TEST_ASSERT_EQUAL(7, chip8.programCounter); // advancing the program counter only once
}

// EXA1
static void test_EXA1_KeyUp_SkipsNextInstructionIfKeyUp(void)
{
    chip8.keypad[8] = false;
    chip8.keypad[2] = true;
    chip8.programCounter = 5;

    ch8_op_KeyNotEquals(&chip8, 0xE8A1);

    TEST_ASSERT_EQUAL(9, chip8.programCounter);
}

static void test_EXA1_KeyUp_DoesNotSkipNextInstructionIfKeyDown(void)
{
    chip8.keypad[4] = true;
    chip8.programCounter = 5;

    ch8_op_KeyNotEquals(&chip8, 0xE4A1);

    TEST_ASSERT_EQUAL(7, chip8.programCounter);
}

// FX07
static void test_FX07_ReadDelayTimer_StoresValueOfDelayTimerInVX(void)
{
    chip8.delayTimer = 56;
    chip8.programCounter = 212;

    ch8_op_ReadDelayTimer(&chip8, 0xF707);

    TEST_ASSERT_EQUAL(56, chip8.V[7]);
    TEST_ASSERT_EQUAL(214, chip8.programCounter);
}

// FX0A
static void test_FX0A_KeyAwait_SetsWaitFlagAndRegister(void)
{
    ch8_op_KeyWait(&chip8, 0xFE0A);
    TEST_ASSERT_TRUE(chip8.waitFlag);
    TEST_ASSERT_EQUAL(0xE, chip8.waitReg);
}

// FX15
static void test_FX15_SetDelayTimer_SetsDelayTimerToValueInVX(void)
{
    chip8.programCounter = 212;
    chip8.V[9] = 44;

    ch8_op_SetDelayTimer(&chip8, 0xF915);

    TEST_ASSERT_EQUAL(44, chip8.delayTimer);
    TEST_ASSERT_EQUAL(214, chip8.programCounter);
}

// FX18
static void test_FX18_SetSoundTimer_SetsSoundTimerToValueInVX(void)
{
    chip8.programCounter = 212;
    chip8.V[9] = 44;

    ch8_op_SetSoundTimer(&chip8, 0xF918);

    TEST_ASSERT_EQUAL(44, chip8.soundTimer);
    TEST_ASSERT_EQUAL(214, chip8.programCounter);
}

// FX1E
static void test_FX1E_AddToIndex_AddsVXToTheIndex(void)
{
    chip8.programCounter = 212;
    chip8.V[4] = 10;
    chip8.V[0xF] = 1;   // should not be affected
    chip8.index = 11;

    ch8_op_AddToIndex(&chip8, 0xF41E);

    TEST_ASSERT_EQUAL(21, chip8.index);
    TEST_ASSERT_EQUAL(1, chip8.V[0xF]);
    TEST_ASSERT_EQUAL(214, chip8.programCounter);
}

// FX33
static void test_FX33_StoreBinaryCodedDecimal_StoresTheCorrectBCDRepresentation(void)
{
    chip8.V[3] = 255;
    chip8.index = 764;
    chip8.programCounter = 212;

    ch8_op_StoreBinaryCodedDecimal(&chip8, 0xF333);

    TEST_ASSERT_EQUAL(2, chip8.memory[chip8.index]);
    TEST_ASSERT_EQUAL(5, chip8.memory[chip8.index + 1]);
    TEST_ASSERT_EQUAL(5, chip8.memory[chip8.index + 2]);
    TEST_ASSERT_EQUAL(214, chip8.programCounter);
}

// FX55
static void test_FX55_Store_StoresV0ToVXInMemory(void)
{
    u16 start_addr = 2078; /* random memory offset */

    chip8.index = start_addr;
    chip8.V[0] = 5;
    chip8.V[1] = 4;
    chip8.V[2] = 22;
    chip8.V[3] = 144;

    chip8.programCounter = 212;

    ch8_op_Store(&chip8, 0xF355);

    TEST_ASSERT_EQUAL(5, chip8.memory[start_addr]);
    TEST_ASSERT_EQUAL(4, chip8.memory[start_addr + 1]);
    TEST_ASSERT_EQUAL(22, chip8.memory[start_addr + 2]);
    TEST_ASSERT_EQUAL(144, chip8.memory[start_addr + 3]);
    TEST_ASSERT_EQUAL(214, chip8.programCounter);

    //TEST_ASSERT_EQUAL(2083, chip8.index_register); /* I = I + x + 1 */
}

// FX65
static void test_FX65_Load_LoadsV0ToVXFromMemory(void)
{
    u16 start_addr = 2078; /* random memory offset */

    chip8.index = start_addr;
    chip8.memory[start_addr] = 5;
    chip8.memory[start_addr + 1] = 4;
    chip8.memory[start_addr + 2] = 22;
    chip8.memory[start_addr + 3] = 144;

    chip8.programCounter = 212;

    ch8_op_Load(&chip8, 0xF365);

    TEST_ASSERT_EQUAL(5, chip8.V[0]);
    TEST_ASSERT_EQUAL(4, chip8.V[1]);
    TEST_ASSERT_EQUAL(22, chip8.V[2]);
    TEST_ASSERT_EQUAL(144, chip8.V[3]);
    TEST_ASSERT_EQUAL(214, chip8.programCounter);

    //TEST_ASSERT_EQUAL(2083, chip8.index_register); /* I = I + x + 1 */
}

int main()
{
    UnityBegin("test/test_opcodes.c");

    // 00E0
    RUN_TEST(test_00E0_ClearScreen_SetsAllPixelsTo0);
    // 00EE
    RUN_TEST(test_00EE_ReturnFromSubroutine_SetsProgramCounterToStackTop);

    // 1NNN
    RUN_TEST(test_1NNN_Jump_SetsProgramCounterToAddress);

    // 2NNN
    RUN_TEST(test_2NNN_CallSub_SetsProgramCounterToAddress);
    RUN_TEST(test_2NNN_CallSub_PushesTheCurrentAddressOntoStack);

    // 3XNN
    RUN_TEST(test_3XNN_SkipEquals_SkipsTheNextInstructionIfEqualNN);
    RUN_TEST(test_3XNN_SkipEquals_DoesNotSkipIfNotEqualNN);

    // 4XNN
    RUN_TEST(test_4XNN_SkipNotEquals_SkipsTheNextInstructionIfNotEqualNN);
    RUN_TEST(test_4XNN_SkipNotEquals_DoesNotSkipNextInstructionIfEqualNN);

    // 5XY0
    RUN_TEST(test_5XYN_SkipVXEqualsVY_SkipsNextInstructionIfVXEqualsVY);
    RUN_TEST(test_5XYN_SkipVXEqualsVY_DoesNotSkipNextInstructionIfVXDoesNotEqualVY);

    // 6XNN
    RUN_TEST(test_6XNN_Set_SetsVXToNN);

    // 7XNN
    RUN_TEST(test_7XNN_Add_AddsNNToVX);

    // 8XY0
    RUN_TEST(test_8XY0_Assign_SetsVXToVY);

    RUN_TEST(test_8XY1_LogicalOr_SetsVXToVXOrVY);
    RUN_TEST(test_8XY2_LogicalAnd_SetsVXToVXAndVY);
    RUN_TEST(test_8XY3_LogicalXor_SetsVXToVXXorVY);

    RUN_TEST(test_8XY4_Add_SetsCarryTo1IfOverflow);
    RUN_TEST(test_8XY4_Add_SetsCarryTo0IfNoOverflow);

    RUN_TEST(test_8XY5_SubtractAssign_SetsBorrowTo0IfUnderflow);
    RUN_TEST(test_8XY5_SubtractAssign_SetsBorrowTo1IfNoUnderflow);

    // 8XY6

    RUN_TEST(test_8XY7_SubtractAssignReverse_SetsBorrowTo0IfUnderflow);
    RUN_TEST(test_8XY7_SubtractAssignReverse_SetsBorrowTo1IfNoUnderflow);

    // 8XYE

    // 9XY0
    RUN_TEST(test_9XYN_SkipVXNotEqualsVY_SkipsNextInstructionIfVXNotEqualsVY);
    RUN_TEST(test_9XYN_SkipVXNotEqualsVY_DoesNotSkipNextInstructionIfVXEqualsVY);

    // ANNN
    RUN_TEST(test_ANNN_SetIndex_SetsTheIndexRegisterToNNN);

    // BXNN
    RUN_TEST(test_BXNN_JumpOffset_SetsPCToNNPlusVX);

    // CXNN
    RUN_TEST(test_CXNN_BitwiseRandom_AdvancesProgramCounter);

    // DXYN
    RUN_TEST(test_DXYN_DrawSprite_SetsTheDrawFlag);

    // E000
    RUN_TEST(test_EX9E_KeyDown_SkipsNextInstructionIfKeyIsDown);
    RUN_TEST(test_EX9E_KeyDown_DoesNotSkipNextInstructionIfKeyUp);
    RUN_TEST(test_EXA1_KeyUp_SkipsNextInstructionIfKeyUp);
    RUN_TEST(test_EXA1_KeyUp_DoesNotSkipNextInstructionIfKeyDown);

    // F000
    RUN_TEST(test_FX07_ReadDelayTimer_StoresValueOfDelayTimerInVX);
    RUN_TEST(test_FX0A_KeyAwait_SetsWaitFlagAndRegister);
    RUN_TEST(test_FX15_SetDelayTimer_SetsDelayTimerToValueInVX);
    RUN_TEST(test_FX18_SetSoundTimer_SetsSoundTimerToValueInVX);
    RUN_TEST(test_FX1E_AddToIndex_AddsVXToTheIndex);
    RUN_TEST(test_FX33_StoreBinaryCodedDecimal_StoresTheCorrectBCDRepresentation);
    RUN_TEST(test_FX55_Store_StoresV0ToVXInMemory);
    RUN_TEST(test_FX65_Load_LoadsV0ToVXFromMemory);

    return UnityEnd();
}
