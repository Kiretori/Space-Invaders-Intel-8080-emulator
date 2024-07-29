#ifndef OPCODES_H
#define OPCODES_H


#include "emu8080.h"

int Parity(uint8_t value);

//================================= Arithmetic instructions: =================================//
void ADD_R(State8080 *state, REGISTERS reg);

void DCR_R(State8080 *state, REGISTERS reg);

void DCR_M(State8080 *state);

void ADD_I(State8080 *state, uint8_t value);

void ADD_M(State8080 *state);

//================================= Branch instructions: =================================//
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

//=================================Stack and I/O instructions: =================================//
void PUSH(State8080 *state, REGISTERS src);

void PUSH_PSW(State8080 *state);

//================================= Data Transfer instructions: =================================//
void MVI_R(State8080 *state, REGISTERS reg, uint8_t byte);

void MVI_M(State8080 *state, uint8_t byte);

void LXI_PAIR(State8080 *state, REGISTERS reg1, REGISTERS reg2, uint8_t byte1, uint8_t byte2);

void LXI_SP(State8080 *state, uint8_t byte1, uint8_t byte2);

void STA(State8080 *state, uint8_t byte1, uint8_t byte2);

//================================= Logical instructions: =================================//
void ANA_R(State8080 *state, REGISTERS reg);

void ANA_M(State8080 *state);

void ANI(State8080 *state, uint8_t byte);

void XRA_R(State8080 *state, REGISTERS reg);

void XRA_M(State8080 *state);

void XRI(State8080 *state, uint8_t byte);

void ORA_R(State8080 *state, REGISTERS reg);

void ORA_M(State8080 *state);

void ORI(State8080 *state, uint8_t byte);

#endif