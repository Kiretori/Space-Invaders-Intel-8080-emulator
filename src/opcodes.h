#ifndef OPCODES_H
#define OPCODES_H


#include "emu8080.h"

int Parity(uint8_t value);

//================================= Arithmetic instructions: =================================//
void ADD_R(State8080 *state, REGISTERS reg);

void ADI(State8080 *state, uint8_t value);

void ADD_M(State8080 *state);

void ADC_R(State8080 *state, REGISTERS reg);

void ADC_M(State8080 *state);

void ACI(State8080 *state, uint8_t byte);

void SUB_R(State8080 *state, REGISTERS reg);

void SUB_M(State8080 *state);

void SUI(State8080 *state, uint8_t byte);

void SBB_R(State8080 *state, REGISTERS reg);

void SBB_M(State8080 *state, REGISTERS reg);

void SBI(State8080 *state, uint8_t byte);

void DCR_R(State8080 *state, REGISTERS reg);

void DCR_M(State8080 *state);

void INR_R(State8080 *state, REGISTERS reg);

void INR_M(State8080 *state);

void INX_PAIR(State8080 *state, REGISTERS reg);

void INX_SP(State8080 *state, REGISTERS reg);

void DCX_PAIR(State8080 *state, REGISTERS reg);

void DCX_SP(State8080 *state);

void DAD(State8080 *state, REGISTERS reg);

void DAA(State8080 *state);
//================================= Branch instructions: =================================//
void CALL(State8080* state, uint8_t byte1, uint8_t byte2);

void CZ(State8080 *state, uint8_t byte1, uint8_t byte2);

void CNZ(State8080 *state, uint8_t byte1, uint8_t byte2);

void CC(State8080 *state, uint8_t byte1, uint8_t byte2);

void CNC(State8080 *state, uint8_t byte1, uint8_t byte2); 

void CPO(State8080 *state, uint8_t byte1, uint8_t byte2);

void CPE(State8080 *state, uint8_t byte1, uint8_t byte2);

void CP(State8080 *state, uint8_t byte1, uint8_t byte2);

void CM(State8080 *state, uint8_t byte1, uint8_t byte2);

void RET(State8080 *state);

void RNZ(State8080 *state);

void RZ(State8080 *state);

void RNC(State8080 *state);

void RC(State8080 *state);

void RPO(State8080 *state);

void RPE(State8080 *state);

void RP(State8080 *state);

void RM(State8080 *state);

void RST_N(State8080 *state, int n);

void PCHL(State8080 *state);

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

void LDA(State8080 *state, uint8_t byte1, uint8_t byte2);

void LDAX(State8080 *state, REGISTERS reg);

void SHLD(State8080 *state, uint8_t byte1, uint8_t byte2);

void LHLD(State8080 *state, uint8_t byte1, uint8_t byte2);

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

void CMP_R(State8080 *state, REGISTERS reg);

void CMP_M(State8080 *state);

void CPI(State8080 *state, uint8_t byte);

void CMA(State8080 *state);

void CMC(State8080 *state);

void STC(State8080 *state);

#endif