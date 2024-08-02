#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>
#include "emu8080.h"
#include "opcodes.h"

const int opcode_cycles[NUM_OPCODES] = {
    4, 10, 7, 5, 5, 5, 7, 4, 4, 10, 7, 5, 5, 5, 7, 4,
    4, 10, 7, 5, 5, 5, 7, 4, 4, 10, 7, 5, 5, 5, 7, 4,
    4, 10, 16, 5, 5, 5, 7, 4, 4, 10, 16, 5, 5, 5, 7, 4,
    4, 10, 13, 5, 10, 10, 10, 4, 4, 10, 13, 5, 5, 5, 7, 4,
    5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,
    5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,
    5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,
    7, 7, 7, 7, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 7, 5,
    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
    5, 10, 10, 10, 11, 11, 7, 11, 5, 10, 10, 10, 11, 17, 7, 11,
    5, 10, 10, 10, 11, 11, 7, 11, 5, 10, 10, 10, 11, 17, 7, 11,
    5, 10, 10, 18, 11, 11, 7, 11, 5, 5, 10, 4, 11, 17, 7, 11,
    5, 10, 10, 4, 11, 11, 7, 11, 5, 5, 10, 4, 11, 17, 7, 11
};

void Reset8080(State8080 *state) {
	// Reseting the state of the CPU
	state->pc = 0;
	state->sp = 0;

	// Reset registers
	for (int i = 0; i < REG_NUMBER; i++) {
		state->registers[i] = 0;
	}

	// Reset memory
	for (int i = 0; i < MAX_MEM; i++) {
		state->memory[i] = 0;
	}

	// Reset flags
	state->cc.z = 0;
	state->cc.s = 0;
	state->cc.p = 0;
	state->cc.cy = 0;
	state->cc.ac = 0;
	state->cc.pad = 0;	

	state->exit = false;
	state->halt = false;
	state->total_cycles = 4;

	state->int_enable = 0;

}


void UnimplimentedInstruction(State8080 *state) {
	printf("Error: Unimplimented instruction\n");
	state->pc--;
	Disassemble8080Op(state->memory, state->pc);
	exit(1);
}



int Emulate8080Op(State8080 *state) {
	uint8_t *opcode = &state->memory[state->pc];
	state->total_cycles += opcode_cycles[*opcode];
	state->pc += 1;

	switch(*opcode) {
		case 0x00:	NOP();	break;
		case 0x01:	LXI_PAIR(state, B, opcode[1], opcode[2]); break;
		case 0x02:	STAX(state, B); 	break;
		case 0x03:	INX_PAIR(state, B);		break;
		case 0x04: 	INR_R(state, B); 		break;
		case 0x05:	DCR_R(state, B);		break;
		case 0x06:  MVI_R(state, B, opcode[1]);	break;
		case 0x07:  RLC(state);			break;
		case 0x08:  UnimplimentedInstruction(state); break;
		case 0x09:	DAD_PAIR(state, B);		break;
		case 0x0A:  LDAX(state, B);		break;
		case 0x0B:	DCX_PAIR(state, B);	break;
		case 0x0C:  INR_R(state, C); 	break;
		case 0x0D:  DCR_R(state, C); 	break;
		case 0x0E:  MVI_R(state, C, opcode[1]); break;
		case 0x0F: 	RRC(state);	break;
		case 0x10:  UnimplimentedInstruction(state); break;
		case 0x11:  LXI_PAIR(state, D, opcode[1], opcode[2]); break;
		case 0x12:  STAX(state, D); 	break;
		case 0x13: 	INX_PAIR(state, D); break;
		case 0x14:  INR_R(state, D);	break;
		case 0x15:  DCR_R(state, D); 	break;
		case 0x16:  MVI_R(state, D, opcode[1]); break;
		case 0x17:  RAL(state);			break;
		case 0x18:  UnimplimentedInstruction(state); break;
		case 0x19:  DAD_PAIR(state, D); 		break;
		case 0x1a:  LDAX(state, D);		break;
		case 0x1b:  DCX_PAIR(state, D); break;
		case 0x1c:  INR_R(state, E);	break;
		case 0x1e:	MVI_R(state, E, opcode[1]); break;
		case 0x1f:  RAR(state);			break;
		case 0x20:  UnimplimentedInstruction(state); break;
		case 0x21:  LXI_PAIR(state, H, opcode[1], opcode[2]); break;
		case 0x22:  SHLD(state, opcode[1], opcode[2]);
		case 0x23:  INX_PAIR(state, H);	break;
		case 0x24:  INR_R(state, H);  	break;
		case 0x25:  DCR_R(state, H); 	break;
		case 0x26:  MVI_R(state, H, opcode[1]); break;
		case 0x27:  DAA(state);			break;
		case 0x28:  UnimplimentedInstruction(state); break;
		case 0x29:  DAD_PAIR(state, H); 		break;
		case 0x2a:  LHLD(state, opcode[1], opcode[2]); break;
		case 0x2b:  DCX_PAIR(state, H); break;
		case 0x2c:  INR_R(state, L); 	break;
		case 0x2d:  DCR_R(state, L); 	break;
		case 0x2e:  MVI_R(state, L, opcode[1]); break;
		case 0x2f:  CMA(state);			break;
		case 0x30:  UnimplimentedInstruction(state); break;
		case 0x31:  LXI_SP(state, opcode[1], opcode[2]); break;
		case 0x32:  STA(state, opcode[1], opcode[2]); break;
		case 0x33:  INX_SP(state);		break;
		case 0x34:  INR_M(state);		break;
		case 0x35:  DCR_M(state);    	break;
		case 0x36:  MVI_M(state, opcode[1]); break;
		case 0x37:	STC(state);			break;
		case 0x38:  UnimplimentedInstruction(state); break;
		case 0x39:	DAD_SP(state); 		break;
		case 0x3a:  LDA(state, opcode[1], opcode[2]); break;
		case 0x3b:  DCX_SP(state);		break;
		case 0x3c:  INR_R(state, A); 	break;
		case 0x3d:  DCR_R(state, A);    break;
		case 0x3e:  MVI_R(state, A, opcode[1]); break;
		case 0x3f:  CMC(state);			break;
		case 0x40:  MOV_R_R(state, B, B); break;
		case 0x41: 	MOV_R_R(state, B, C); break;
		case 0x42:  MOV_R_R(state, B, D); break;
		case 0x43:  MOV_R_R(state, B, E); break;
		case 0x44:  MOV_R_R(state, B, H); break;
		case 0x45:  MOV_R_R(state, B, L); break;
		case 0x46:  MOV_R_M(state, B);	break;
		case 0x47:  MOV_R_R(state, B, A); break;
		case 0x48:  MOV_R_R(state, C, B);  break;
		case 0x49:  MOV_R_R(state, C, C);  break;
		case 0x4a:  MOV_R_R(state, C, D);  break;
		case 0x4b:  MOV_R_R(state, C, E);  break;
		case 0x4c:  MOV_R_R(state, C, H);  break;
		case 0x4d:  MOV_R_R(state, C, L);  break;
		case 0x4e:  MOV_R_M(state, C);	break;
		case 0x4f:  MOV_R_R(state, C, A); break;
		case 0x50:  MOV_R_R(state, D, B); break;
		case 0x51:  MOV_R_R(state, D, C); break;
		case 0x52:  MOV_R_R(state, D, D); break;
		case 0x53:  MOV_R_R(state, D, E); break;
		case 0x54:  MOV_R_R(state, D, H); break;
		case 0x55:  MOV_R_R(state, D, L); break;
		case 0x56:  MOV_R_M(state, D);	break;
		case 0x57:  MOV_R_R(state, D, A); break;
		case 0x58:  MOV_R_R(state, E, B); break;
		case 0x59:  MOV_R_R(state, E, C); break;
		case 0x5a:  MOV_R_R(state, E, D); break;
		case 0x5b:  MOV_R_R(state, E, E); break;
		case 0x5c:  MOV_R_R(state, E, H); break;
		case 0x5d:  MOV_R_R(state, E, L); break;
		case 0x5e:  MOV_R_M(state, E);	break;
		case 0x5f:  MOV_R_R(state, E, A); break;
		case 0x60:  MOV_R_R(state, H, B); break;
		case 0x61:  MOV_R_R(state, H, C); break;
		case 0x62:  MOV_R_R(state, H, D); break;
		case 0x63:  MOV_R_R(state, H, E); break;
		case 0x64:  MOV_R_R(state, H, H); break;
		case 0x65:  MOV_R_R(state, H, L); break;
		case 0x66:  MOV_R_M(state, H); 	break;
		case 0x67:  MOV_R_R(state, H, A); break;
		case 0x68:  MOV_R_R(state, L, B); break;
		case 0x69:  MOV_R_R(state, L, C); break;
		case 0x6a:  MOV_R_R(state, L, D); break;
		case 0x6b:  MOV_R_R(state, L, E); break;
		case 0x6c:  MOV_R_R(state, L, H); break;
		case 0x6d:  MOV_R_R(state, L, L); break;
		case 0x6e:  MOV_R_M(state, L);	break;
		case 0x6f:	MOV_R_R(state, L, A);	break;
		case 0x70:  MOV_M_R(state, B);	break;
		case 0x71:  MOV_M_R(state, C);	break;
		case 0x72:  MOV_M_R(state, D);	break;
		case 0x73:  MOV_M_R(state, E);	break;
		case 0x74:  MOV_M_R(state, H);	break;
		case 0x75:  MOV_M_R(state, L);	break;
		case 0x76:  HLT(state);			break;
		case 0x77:  MOV_M_R(state, A);	break;
		case 0x78:  MOV_R_R(state, A, B); break;
		case 0x79:  MOV_R_R(state, A, C); break;
		case 0x7a:  MOV_R_R(state, A, D); break;
		case 0x7b:  MOV_R_R(state, A, E); break;
		case 0x7c:  MOV_R_R(state, A, H); break;
		case 0x7d:  MOV_R_R(state, A, L); break;
		case 0x7e:  MOV_R_M(state, A);	break;
		case 0x7f:  MOV_R_R(state, A, A); break;
		case 0x80:  ADD_R(state, B);	break;
		case 0x81:  ADD_R(state, C);	break;
		case 0x82:  ADD_R(state, D);	break;
		case 0x83:  ADD_R(state, E);	break;
		case 0x84:  ADD_R(state, H);	break;
		case 0x85:  ADD_R(state, L);	break;
		case 0x86:  ADD_M(state); 		break;
		case 0x87:  ADD_R(state, A); 	break;
		case 0x88:  ADC_R(state, B); 	break;
		case 0x89:  ADC_R(state, C); 	break;
		case 0x8a:  ADC_R(state, D); 	break;
		case 0x8b:  ADC_R(state, E); 	break;
		case 0x8c:  ADC_R(state, H); 	break;
		case 0x8d:  ADC_R(state, L); 	break;
		case 0x8e:  ADC_M(state); 		break;
		case 0x8f:  ADC_R(state, A); 	break;
		case 0x90:  SUB_R(state, B); 	break;
		case 0x91:  SUB_R(state, C); 	break;
		case 0x92:  SUB_R(state, D); 	break;
		case 0x93:  SUB_R(state, E); 	break;
		case 0x94:  SUB_R(state, H); 	break;
		case 0x95:  SUB_R(state, L); 	break;
		case 0x96:  SUB_M(state); 		break;
		case 0x97:  SUB_R(state, A);  	break;
		case 0x98:  SBB_R(state, B); 	break;
		case 0x99:  SBB_R(state, C); 	break;
		case 0x9a:  SBB_R(state, D); 	break;
		case 0x9b:  SBB_R(state, E); 	break;
		case 0x9c:  SBB_R(state, H); 	break;
		case 0x9d:  SBB_R(state, L); 	break;
		case 0x9e:  SBB_M(state); 		break;
		case 0x9f:  SBB_R(state, A); 	break;
		case 0xa0:  ANA_R(state, B); 	break;
		case 0xa1:  ANA_R(state, C); 	break;
		case 0xa2:  ANA_R(state, D); 	break;
		case 0xa3:  ANA_R(state, E); 	break;
		case 0xa4:  ANA_R(state, H); 	break;
		case 0xa5:  ANA_R(state, L); 	break;
		case 0xa6:  ANA_M(state); 		break;
		case 0xa7:  ANA_R(state, A); 	break;
		case 0xa8:  XRA_R(state, B); 	break;
		case 0xa9:  XRA_R(state, C); 	break;
		case 0xaa:  XRA_R(state, D); 	break;
		case 0xab:  XRA_R(state, E); 	break;
		case 0xac:  XRA_R(state, H); 	break;
		case 0xad:  XRA_R(state, L); 	break;
		case 0xae:  XRA_M(state); 		break;
		case 0xaf:  XRA_R(state, A); 	break;
		case 0xb0:  ORA_R(state, B); 	break;
		case 0xb1:  ORA_R(state, C); 	break;
		case 0xb2:  ORA_R(state, D); 	break;
		case 0xb3:  ORA_R(state, E); 	break;
		case 0xb4:  ORA_R(state, H); 	break;
		case 0xb5:  ORA_R(state, L); 	break;
		case 0xb6:  ORA_M(state); 		break;
		case 0xb7:  ORA_R(state, A); 	break;
		case 0xb8:  CMP_R(state, B); 	break;
		case 0xb9:  CMP_R(state, C); 	break;
		case 0xba:  CMP_R(state, D); 	break;
		case 0xbb:  CMP_R(state, E); 	break;
		case 0xbc:  CMP_R(state, H); 	break;
		case 0xbd:  CMP_R(state, L); 	break;
		case 0xbe:  CMP_M(state); 		break;
		case 0xbf:  CMP_R(state, A); 	break;
		case 0xc0:  RNZ(state); 		break;
		case 0xc1:	POP(state, B); 		break;
		case 0xc2:  JNZ(state, opcode[1], opcode[2]); break;
		case 0xc3:  JMP(state, opcode[1], opcode[2]); break;
		case 0xc4:  CNZ(state, opcode[1], opcode[2]); break;
		case 0xc5:  PUSH(state, B); 	break;
		case 0xc6:  ADI(state, opcode[1]); break;
		case 0xc7:  RST_N(state, 0); 	break;
		case 0xc8:  RZ(state); 			break;
		case 0xc9:  RET(state); 		break;
		case 0xca: 	JZ(state, opcode[1], opcode[2]); break;
		case 0xcb:  UnimplimentedInstruction(state); break;
		case 0xcc:	CZ(state, opcode[1], opcode[2]); break;
		case 0xcd:	CALL(state, opcode[1], opcode[2]); break;
		case 0xce:  ACI(state, opcode[1]); break;
		case 0xcf:  RST_N(state, 1); 	break;
		case 0xd0: 	RNC(state); 		break;
		case 0xd1:	POP(state, D); 		break;
		case 0xd2:	JNC(state, opcode[1], opcode[2]); break;
		case 0xd3:	OUT(state, opcode[1]); break;
		case 0xd4:	CNC(state, opcode[1], opcode[2]); break;
		case 0xd5:  PUSH(state, D); 	break;
		case 0xd6:	SUI(state, opcode[1]); break;
		case 0xd7:	RST_N(state, 2); 	break;
		case 0xd8:	RC(state); 			break;
		case 0xd9:  UnimplimentedInstruction(state); break;
		case 0xda:  JC(state, opcode[1], opcode[2]); break;
		case 0xdb:  IN(state, opcode[1]); break;
		case 0xdc:	CC(state, opcode[1], opcode[2]); break;
		case 0xdd:  UnimplimentedInstruction(state); break;
		case 0xde:  SBI(state, opcode[1]); break;
		case 0xdf:  RST_N(state, 3);	break;
		case 0xe0:  RPO(state); 		break;
		case 0xe1:  POP(state, H); 		break;
		case 0xe2:  JPO(state, opcode[1], opcode[2]); break;
		case 0xe3:  XTHL(state); 		break;
		case 0xe4:	CPO(state, opcode[1], opcode[2]); break;
		case 0xe5:  PUSH(state, H);		break;
		case 0xe6:	ANI(state, opcode[1]); break;
		case 0xe7:  RST_N(state, 4); 	break;
		case 0xe8:	RPE(state); 		break;
		case 0xe9:	PCHL(state); 		break;
		case 0xea:  JPE(state, opcode[1], opcode[2]); break;
		case 0xeb:  XCHG(state); 		break;
		case 0xec:  CPE(state, opcode[1], opcode[2]); break;
		case 0xed:  UnimplimentedInstruction(state); break;
		case 0xee:  XRI(state, opcode[1]); break;
		case 0xef:  RST_N(state, 5); 	break;
		case 0xf0:  RP(state); 			break;
		case 0xf1:  POP_PSW(state);		break;
		case 0xf2:  JP(state, opcode[1], opcode[2]); break;
		case 0xf3:	DI(state); 			break;
		case 0xf4:  CP(state, opcode[1], opcode[2]); break;
		case 0xf5:  PUSH_PSW(state); 	break;
		case 0xf6:  ORI(state, opcode[1]); break;
		case 0xf7:  RST_N(state, 6); 	break;
		case 0xf8:	RM(state);			break;
		case 0xf9:	SPHL(state);		break;
		case 0xfa:	JM(state, opcode[1], opcode[2]); break;
		case 0xfb: 	EI(state); 			break;
		case 0xfc: 	CM(state, opcode[1], opcode[2]); break;
		case 0xfd:  UnimplimentedInstruction(state); break;
		case 0xfe:  CPI(state, opcode[1]); break;
		case 0xff:  RST_N(state, 7); 	break;
	}
}


int Disassemble8080Op(unsigned char *codebuffer, int pc)
{
    unsigned char *code = &codebuffer[pc];
    int opbytes = 1;
    printf("%04x ", pc);

    switch(*code)
    {
        case 0x00: printf("NOP"); break;
        case 0x01: printf("LXI    B, #$%02x%02x", code[2], code[1]); opbytes = 3; break;
        case 0x02: printf("STAX   B"); break;
        case 0x03: printf("INX    B"); break;
        case 0x04: printf("INR    B"); break;
        case 0x05: printf("DCR    B"); break;
        case 0x06: printf("MVI    B, #$%02x", code[1]); opbytes = 2; break;
        case 0x07: printf("RLC"); break;
        case 0x08: printf("NOP"); break;
        case 0x09: printf("DAD    B"); break;
        case 0x0a: printf("LDAX   B"); break;
        case 0x0b: printf("DCX    B"); break;
        case 0x0c: printf("INR    C"); break;
        case 0x0d: printf("DCR    C"); break;
        case 0x0e: printf("MVI    C, #$%02x", code[1]); opbytes = 2; break;
        case 0x0f: printf("RRC"); break;
        case 0x10: printf("NOP"); break;
        case 0x11: printf("LXI    D, #$%02x%02x", code[2], code[1]); opbytes = 3; break;
        case 0x12: printf("STAX   D"); break;
        case 0x13: printf("INX    D"); break; 
        case 0x14: printf("INR    D"); break;
        case 0x15: printf("DCR    D"); break;
        case 0x16: printf("MVI    D, #$%02x", code[1]); opbytes = 2; break;
        case 0x17: printf("RAL"); break;
        case 0x18: printf("NOP"); break;
        case 0x19: printf("DAD    D"); break;
        case 0x1a: printf("LDAX   D"); break;
        case 0x1b: printf("DCX    D"); break;
        case 0x1c: printf("INR    E"); break;
        case 0x1d: printf("DCR    E"); break;
        case 0x1e: printf("MVI    E, #$%02x", code[1]); opbytes = 2; break;
        case 0x1f: printf("RAR"); break;
        case 0x20: printf("NOP"); break;
        case 0x21: printf("LXI    H, #$%02x%02x", code[2], code[1]); opbytes = 3; break;
        case 0x22: printf("SHLD   #$%02x%02x", code[2], code[1]); opbytes = 3; break;
        case 0x23: printf("INX    H"); break; 
        case 0x24: printf("INR    H"); break;
        case 0x25: printf("DCR    H"); break;
        case 0x26: printf("MVI    H, #$%02x", code[1]); opbytes = 2; break;
        case 0x27: printf("DAA"); break;
        case 0x28: printf("NOP"); break;
        case 0x29: printf("DAD    H"); break;
        case 0x2a: printf("LHLD $%02x%02x", code[2], code[1]); opbytes = 3; break;
        case 0x2b: printf("DCX    H"); break;
		case 0x2c: printf("INR    L"); break;
		case 0x2d: printf("DCR    L"); break;
		case 0x2e: printf("MVI    L,#$%02x", code[1]); opbytes = 2; break;
		case 0x2f: printf("CMA"); break;
			
		case 0x30: printf("NOP"); break;
		case 0x31: printf("LXI    SP,#$%02x%02x", code[2], code[1]); opbytes=3; break;
		case 0x32: printf("STA    $%02x%02x", code[2], code[1]); opbytes=3; break;
		case 0x33: printf("INX    SP"); break;
		case 0x34: printf("INR    M"); break;
		case 0x35: printf("DCR    M"); break;
		case 0x36: printf("MVI    M,#$%02x", code[1]); opbytes=2; break;
		case 0x37: printf("STC"); break;
		case 0x38: printf("NOP"); break;
		case 0x39: printf("DAD    SP"); break;
		case 0x3a: printf("LDA    $%02x%02x", code[2], code[1]); opbytes=3; break;
		case 0x3b: printf("DCX    SP"); break;
		case 0x3c: printf("INR    A"); break;
		case 0x3d: printf("DCR    A"); break;
		case 0x3e: printf("MVI    A,#$%02x", code[1]); opbytes = 2; break;
		case 0x3f: printf("CMC"); break;
			
		case 0x40: printf("MOV    B,B"); break;
		case 0x41: printf("MOV    B,C"); break;
		case 0x42: printf("MOV    B,D"); break;
		case 0x43: printf("MOV    B,E"); break;
		case 0x44: printf("MOV    B,H"); break;
		case 0x45: printf("MOV    B,L"); break;
		case 0x46: printf("MOV    B,M"); break;
		case 0x47: printf("MOV    B,A"); break;
		case 0x48: printf("MOV    C,B"); break;
		case 0x49: printf("MOV    C,C"); break;
		case 0x4a: printf("MOV    C,D"); break;
		case 0x4b: printf("MOV    C,E"); break;
		case 0x4c: printf("MOV    C,H"); break;
		case 0x4d: printf("MOV    C,L"); break;
		case 0x4e: printf("MOV    C,M"); break;
		case 0x4f: printf("MOV    C,A"); break;
			
		case 0x50: printf("MOV    D,B"); break;
		case 0x51: printf("MOV    D,C"); break;
		case 0x52: printf("MOV    D,D"); break;
		case 0x53: printf("MOV    D.E"); break;
		case 0x54: printf("MOV    D,H"); break;
		case 0x55: printf("MOV    D,L"); break;
		case 0x56: printf("MOV    D,M"); break;
		case 0x57: printf("MOV    D,A"); break;
		case 0x58: printf("MOV    E,B"); break;
		case 0x59: printf("MOV    E,C"); break;
		case 0x5a: printf("MOV    E,D"); break;
		case 0x5b: printf("MOV    E,E"); break;
		case 0x5c: printf("MOV    E,H"); break;
		case 0x5d: printf("MOV    E,L"); break;
		case 0x5e: printf("MOV    E,M"); break;
		case 0x5f: printf("MOV    E,A"); break;

		case 0x60: printf("MOV    H,B"); break;
		case 0x61: printf("MOV    H,C"); break;
		case 0x62: printf("MOV    H,D"); break;
		case 0x63: printf("MOV    H.E"); break;
		case 0x64: printf("MOV    H,H"); break;
		case 0x65: printf("MOV    H,L"); break;
		case 0x66: printf("MOV    H,M"); break;
		case 0x67: printf("MOV    H,A"); break;
		case 0x68: printf("MOV    L,B"); break;
		case 0x69: printf("MOV    L,C"); break;
		case 0x6a: printf("MOV    L,D"); break;
		case 0x6b: printf("MOV    L,E"); break;
		case 0x6c: printf("MOV    L,H"); break;
		case 0x6d: printf("MOV    L,L"); break;
		case 0x6e: printf("MOV    L,M"); break;
		case 0x6f: printf("MOV    L,A"); break;

		case 0x70: printf("MOV    M,B"); break;
		case 0x71: printf("MOV    M,C"); break;
		case 0x72: printf("MOV    M,D"); break;
		case 0x73: printf("MOV    M.E"); break;
		case 0x74: printf("MOV    M,H"); break;
		case 0x75: printf("MOV    M,L"); break;
		case 0x76: printf("HLT");        break;
		case 0x77: printf("MOV    M,A"); break;
		case 0x78: printf("MOV    A,B"); break;
		case 0x79: printf("MOV    A,C"); break;
		case 0x7a: printf("MOV    A,D"); break;
		case 0x7b: printf("MOV    A,E"); break;
		case 0x7c: printf("MOV    A,H"); break;
		case 0x7d: printf("MOV    A,L"); break;
		case 0x7e: printf("MOV    A,M"); break;
		case 0x7f: printf("MOV    A,A"); break;

		case 0x80: printf("ADD    B"); break;
		case 0x81: printf("ADD    C"); break;
		case 0x82: printf("ADD    D"); break;
		case 0x83: printf("ADD    E"); break;
		case 0x84: printf("ADD    H"); break;
		case 0x85: printf("ADD    L"); break;
		case 0x86: printf("ADD    M"); break;
		case 0x87: printf("ADD    A"); break;
		case 0x88: printf("ADC    B"); break;
		case 0x89: printf("ADC    C"); break;
		case 0x8a: printf("ADC    D"); break;
		case 0x8b: printf("ADC    E"); break;
		case 0x8c: printf("ADC    H"); break;
		case 0x8d: printf("ADC    L"); break;
		case 0x8e: printf("ADC    M"); break;
		case 0x8f: printf("ADC    A"); break;

		case 0x90: printf("SUB    B"); break;
		case 0x91: printf("SUB    C"); break;
		case 0x92: printf("SUB    D"); break;
		case 0x93: printf("SUB    E"); break;
		case 0x94: printf("SUB    H"); break;
		case 0x95: printf("SUB    L"); break;
		case 0x96: printf("SUB    M"); break;
		case 0x97: printf("SUB    A"); break;
		case 0x98: printf("SBB    B"); break;
		case 0x99: printf("SBB    C"); break;
		case 0x9a: printf("SBB    D"); break;
		case 0x9b: printf("SBB    E"); break;
		case 0x9c: printf("SBB    H"); break;
		case 0x9d: printf("SBB    L"); break;
		case 0x9e: printf("SBB    M"); break;
		case 0x9f: printf("SBB    A"); break;

		case 0xa0: printf("ANA    B"); break;
		case 0xa1: printf("ANA    C"); break;
		case 0xa2: printf("ANA    D"); break;
		case 0xa3: printf("ANA    E"); break;
		case 0xa4: printf("ANA    H"); break;
		case 0xa5: printf("ANA    L"); break;
		case 0xa6: printf("ANA    M"); break;
		case 0xa7: printf("ANA    A"); break;
		case 0xa8: printf("XRA    B"); break;
		case 0xa9: printf("XRA    C"); break;
		case 0xaa: printf("XRA    D"); break;
		case 0xab: printf("XRA    E"); break;
		case 0xac: printf("XRA    H"); break;
		case 0xad: printf("XRA    L"); break;
		case 0xae: printf("XRA    M"); break;
		case 0xaf: printf("XRA    A"); break;

		case 0xb0: printf("ORA    B"); break;
		case 0xb1: printf("ORA    C"); break;
		case 0xb2: printf("ORA    D"); break;
		case 0xb3: printf("ORA    E"); break;
		case 0xb4: printf("ORA    H"); break;
		case 0xb5: printf("ORA    L"); break;
		case 0xb6: printf("ORA    M"); break;
		case 0xb7: printf("ORA    A"); break;
		case 0xb8: printf("CMP    B"); break;
		case 0xb9: printf("CMP    C"); break;
		case 0xba: printf("CMP    D"); break;
		case 0xbb: printf("CMP    E"); break;
		case 0xbc: printf("CMP    H"); break;
		case 0xbd: printf("CMP    L"); break;
		case 0xbe: printf("CMP    M"); break;
		case 0xbf: printf("CMP    A"); break;

		case 0xc0: printf("RNZ"); break;
		case 0xc1: printf("POP    B"); break;
		case 0xc2: printf("JNZ    $%02x%02x",code[2],code[1]); opbytes = 3; break;
		case 0xc3: printf("JMP    $%02x%02x",code[2],code[1]); opbytes = 3; break;
		case 0xc4: printf("CNZ    $%02x%02x",code[2],code[1]); opbytes = 3; break;
		case 0xc5: printf("PUSH   B"); break;
		case 0xc6: printf("ADI    #$%02x",code[1]); opbytes = 2; break;
		case 0xc7: printf("RST    0"); break;
		case 0xc8: printf("RZ"); break;
		case 0xc9: printf("RET"); break;
		case 0xca: printf("JZ     $%02x%02x",code[2],code[1]); opbytes = 3; break;
		case 0xcb: printf("JMP    $%02x%02x",code[2],code[1]); opbytes = 3; break;
		case 0xcc: printf("CZ     $%02x%02x",code[2],code[1]); opbytes = 3; break;
		case 0xcd: printf("CALL   $%02x%02x",code[2],code[1]); opbytes = 3; break;
		case 0xce: printf("ACI    #$%02x",code[1]); opbytes = 2; break;
		case 0xcf: printf("RST    1"); break;

		case 0xd0: printf("RNC"); break;
		case 0xd1: printf("POP    D"); break;
		case 0xd2: printf("JNC    $%02x%02x",code[2],code[1]); opbytes = 3; break;
		case 0xd3: printf("OUT    #$%02x",code[1]); opbytes = 2; break;
		case 0xd4: printf("CNC    $%02x%02x",code[2],code[1]); opbytes = 3; break;
		case 0xd5: printf("PUSH   D"); break;
		case 0xd6: printf("SUI    #$%02x",code[1]); opbytes = 2; break;
		case 0xd7: printf("RST    2"); break;
		case 0xd8: printf("RC");  break;
		case 0xd9: printf("RET"); break;
		case 0xda: printf("JC     $%02x%02x",code[2],code[1]); opbytes = 3; break;
		case 0xdb: printf("IN     #$%02x",code[1]); opbytes = 2; break;
		case 0xdc: printf("CC     $%02x%02x",code[2],code[1]); opbytes = 3; break;
		case 0xdd: printf("CALL   $%02x%02x",code[2],code[1]); opbytes = 3; break;
		case 0xde: printf("SBI    #$%02x",code[1]); opbytes = 2; break;
		case 0xdf: printf("RST    3"); break;

		case 0xe0: printf("RPO"); break;
		case 0xe1: printf("POP    H"); break;
		case 0xe2: printf("JPO    $%02x%02x",code[2],code[1]); opbytes = 3; break;
		case 0xe3: printf("XTHL");break;
		case 0xe4: printf("CPO    $%02x%02x",code[2],code[1]); opbytes = 3; break;
		case 0xe5: printf("PUSH   H"); break;
		case 0xe6: printf("ANI    #$%02x",code[1]); opbytes = 2; break;
		case 0xe7: printf("RST    4"); break;
		case 0xe8: printf("RPE"); break;
		case 0xe9: printf("PCHL");break;
		case 0xea: printf("JPE    $%02x%02x",code[2],code[1]); opbytes = 3; break;
		case 0xeb: printf("XCHG"); break;
		case 0xec: printf("CPE     $%02x%02x",code[2],code[1]); opbytes = 3; break;
		case 0xed: printf("CALL   $%02x%02x",code[2],code[1]); opbytes = 3; break;
		case 0xee: printf("XRI    #$%02x",code[1]); opbytes = 2; break;
		case 0xef: printf("RST    5"); break;

		case 0xf0: printf("RP");  break;
		case 0xf1: printf("POP    PSW"); break;
		case 0xf2: printf("JP     $%02x%02x",code[2],code[1]); opbytes = 3; break;
		case 0xf3: printf("DI");  break;
		case 0xf4: printf("CP     $%02x%02x",code[2],code[1]); opbytes = 3; break;
		case 0xf5: printf("PUSH   PSW"); break;
		case 0xf6: printf("ORI    #$%02x",code[1]); opbytes = 2; break;
		case 0xf7: printf("RST    6"); break;
		case 0xf8: printf("RM");  break;
		case 0xf9: printf("SPHL");break;
		case 0xfa: printf("JM     $%02x%02x",code[2],code[1]); opbytes = 3; break;
		case 0xfb: printf("EI");  break;
		case 0xfc: printf("CM     $%02x%02x",code[2],code[1]); opbytes = 3; break;
		case 0xfd: printf("CALL   $%02x%02x",code[2],code[1]); opbytes = 3; break;
		case 0xfe: printf("CPI    #$%02x",code[1]); opbytes = 2; break;
		case 0xff: printf("RST    7"); break;

    }
	printf("\n");
	return opbytes;
}








#ifndef TESTING
int main(int argc, char **argv) {
    FILE* code_file = fopen(argv[1], "rb");
	if (code_file == NULL) {
		printf("There was an error opening %s\n", argv[1]);
		exit(1);
	}
	
	fseek(code_file, 0L, SEEK_END);
	int cf_size = ftell(code_file);
	fseek(code_file, 0L, SEEK_SET);

	unsigned char *buffer = malloc(cf_size);

	fread(buffer, 1, cf_size, code_file);
	fclose(code_file);

	State8080 *cpu = malloc(sizeof(State8080));
	Reset8080(cpu);
	
	while (cpu->pc < cf_size) {
		Disassemble8080Op(buffer, cpu->pc);
		Emulate8080Op(cpu);
	}

	return 0;
}
#endif 