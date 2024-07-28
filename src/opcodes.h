#ifndef OPCODES_H
#define OPCODES_H


#include "emu8080.h"

int Parity(uint8_t value);

void ADD_R(State8080 *state, REGISTERS reg);

void STA(State8080 *state, uint8_t byte1, uint8_t byte2);

void DCR_R(State8080 *state, REGISTERS reg);

void DCR_M(State8080 *state);

void ADD_I(State8080 *state, uint8_t value);

void ADD_M(State8080 *state);

void CALL(State8080* state, uint8_t byte1, uint8_t byte2);

void JMP(State8080 *state, uint8_t byte1, uint8_t byte2);

void JNZ(State8080 *state, uint8_t byte1, uint8_t byte2);

void JZ(State8080 *state, uint8_t byte1, uint8_t byte2);

void JC(State8080 *state, uint8_t byte1, uint8_t byte2);

void JNC(State8080 *state, uint8_t byte1, uint8_t byte2);

void JM(State8080 *state, uint8_t byte1, uint8_t byte2);

void JP(State8080 *state, uint8_t byte1, uint8_t byte2);

void JPE(State8080 *state, uint8_t byte1, uint8_t byte2);

void JPO(State8080 *state, uint8_t byte1, uint8_t byte2);

void PUSH_R(State8080 *state, REGISTERS src);

void PUSH_PSW(State8080 *state);

void MVI_R(State8080 *state, REGISTERS reg, uint8_t byte);

void MVI_M(State8080 *state, uint8_t byte);

void ANA_R(State8080 *state, REGISTERS reg);

void ANA_M(State8080 *state);

void LXI_PAIR(State8080 *state, REGISTERS reg1, REGISTERS reg2, uint8_t byte1, uint8_t byte2);

void LXI_SP(State8080 *state, uint8_t byte1, uint8_t byte2);


#endif