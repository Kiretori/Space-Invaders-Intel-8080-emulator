#include <stdlib.h>
#include <stdio.h>
#include "unity.h"
#include "emu8080.h"
#include "opcodes.h"

State8080 *cpu;

void setUp(void) {
    cpu = malloc(sizeof(State8080));
    if (cpu == NULL) {
        TEST_FAIL_MESSAGE("Failed to allocate memory for CPU\n");
    }

    Reset8080(cpu);

}

void tearDown(void) {
    free(cpu);
}

void test_STA(void) {
    cpu->registers[A] = 0x69;
    STA(cpu, 0x25, 0x77);
    TEST_ASSERT_EQUAL_HEX8(0x69, cpu->memory[0x7725]);
}


void test_DCR_R(void) {
    cpu->registers[B] = 0x00;
    DCR_R(cpu, B);

    TEST_ASSERT_EQUAL_HEX8(0xFF, cpu->registers[B]);
    TEST_ASSERT_BITS(1, 0, cpu->cc.z);
    TEST_ASSERT_BITS(1, 1, cpu->cc.s);
    TEST_ASSERT_BITS(1, Parity(0xFF), cpu->cc.p);
    TEST_ASSERT_BITS(1, 0, cpu->cc.ac);
}

void test_DCR_M(void) {
    cpu->registers[H] = 0x58;
    cpu->registers[L] = 0x11;
    cpu->memory[0x5811] = 0x01;

    DCR_M(cpu);

    TEST_ASSERT_EQUAL_HEX8(0x00, cpu->memory[0x5811]);
    TEST_ASSERT_BITS(1, 1, cpu->cc.z);
    TEST_ASSERT_BITS(1, 0, cpu->cc.s);
    TEST_ASSERT_BITS(1, 1, cpu->cc.p);
    TEST_ASSERT_BITS(1, 1, cpu->cc.ac);
}


void test_ADD_I(void) {
    cpu->registers[A] = 0x25;
    ADD_I(cpu, 0x03);

    TEST_ASSERT_EQUAL_HEX8(0x28, cpu->registers[A]);
    TEST_ASSERT_BITS(1, 0, cpu->cc.z);
    TEST_ASSERT_BITS(1, 0, cpu->cc.s);
    TEST_ASSERT_BITS(1, 1, cpu->cc.p);
    TEST_ASSERT_BITS(1, 0, cpu->cc.ac);

}


void test_ADD_R(void) {
    cpu->registers[A] = 0xF1;
    cpu->registers[B] = 0x42;
    ADD_R(cpu, B);

    TEST_ASSERT_EQUAL_HEX8(0x33, cpu->registers[A]);
    TEST_ASSERT_BITS(1, 0, cpu->cc.z);
    TEST_ASSERT_BITS(1, 0, cpu->cc.s);
    TEST_ASSERT_BITS(1, 1, cpu->cc.p);
    TEST_ASSERT_BITS(1, 1, cpu->cc.cy);
    TEST_ASSERT_BITS(1, 0, cpu->cc.ac);

}

void test_ADD_M(void) {
    cpu->registers[H] = 0x11;
    cpu->registers[L] = 0x2F;
    cpu->registers[A] = 0x00;
    cpu->memory[0x112F] = 0x01;

    ADD_M(cpu);

    TEST_ASSERT_EQUAL_HEX8(0x01, cpu->registers[A]);
    TEST_ASSERT_BITS(1, 0, cpu->cc.z);
    TEST_ASSERT_BITS(1, 0, cpu->cc.s);
    TEST_ASSERT_BITS(1, 0, cpu->cc.p);
    TEST_ASSERT_BITS(1, 0, cpu->cc.cy);
    TEST_ASSERT_BITS(1, 0, cpu->cc.ac);
}

void test_JMP(void) {
    cpu->pc = 0x0515;
    JMP(cpu, 0x12, 0x45);
    TEST_ASSERT_EQUAL_HEX16(0x4512, cpu->pc);
    JMP(cpu, 0x00, 0x00);
    TEST_ASSERT_EQUAL_HEX16(0x0000, cpu->pc);
}

void test_MVI_R(void) {
    MVI_R(cpu, C, 0x28);

    TEST_ASSERT_EQUAL_HEX8(0x28, cpu->registers[C]);

}

void test_LXI_PAIR(void) {
    LXI_PAIR(cpu, B, C, 0x26, 0x11);

    TEST_ASSERT_EQUAL_HEX8(0x26, cpu->registers[C]);
    TEST_ASSERT_EQUAL_HEX8(0x11, cpu->registers[B]);

}

void test_LXI_SP(void) {
    LXI_SP(cpu, 0x15, 0xFE);
    TEST_ASSERT_EQUAL_HEX16(0xFE15, cpu->sp);
}

#ifdef TESTING

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_STA);
    RUN_TEST(test_DCR_R);
    RUN_TEST(test_DCR_M);
    RUN_TEST(test_ADD_I);
    RUN_TEST(test_ADD_R);
    RUN_TEST(test_ADD_M);
    RUN_TEST(test_JMP);
    RUN_TEST(test_MVI_R);
    RUN_TEST(test_LXI_PAIR);
    RUN_TEST(test_LXI_SP);
    return UNITY_END();
}
#endif