#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "emu8080.h"
#include "opcodes.h"

int Parity(uint8_t value) {
    int set_bits = 0;
    while(value) {
        set_bits += value & 1;
        value >>= 1; 
    }
    return !(set_bits % 2);
}

static void _cpu_set_reg_pair(State8080 *state, REGISTERS reg1, REGISTERS reg2, uint8_t byte1, uint8_t byte2) {
    state->registers[reg1] = byte2;
    state->registers[reg2] = byte1;
}

static void _update_flag_z(State8080 *state, uint16_t res) {
    state->cc.z = ((res & 0xFF) == 0);
}

static void _update_flag_s(State8080 *state, uint16_t res) {
    res = res & 0xFF;
    state->cc.s = ((res & (1 << 7)) != 0);
}


static void _update_flag_p(State8080 *state, uint16_t res) {
    state->cc.p = Parity(res & 0xFF);
}

static void _update_flag_cy_sub(State8080 *state, uint8_t val1, uint8_t val2, bool sub_borrow) {
    int borrow = sub_borrow ? state->cc.cy : 0;
    uint16_t res = val1 + ~val2 + !borrow;
    state->cc.cy = res > 0xFF;
}

static void _update_flag_cy_add_16(State8080 *state, uint32_t answer) {
    state->cc.cy = (answer > 0xFFFF);
}

static void _update_flag_cy_add(State8080 *state, uint16_t answer, bool add_carry) {
    int carry = add_carry ? state->cc.cy : 0;
    uint16_t res = answer + carry;
    state->cc.cy = res > 0xFF;
}

static void _update_flag_ac_sub(State8080 *state, uint8_t val1, uint8_t val2, bool sub_borrow) {
    int borrow = sub_borrow ? state->cc.cy : 0;
    val1 &= 0xF;
    val2 = (~val2) & 0xF;
    state->cc.ac = ((val1 + val2 + !borrow) > 0xF);
}

static void _update_flag_ac_add(State8080 *state, uint8_t val1, uint8_t val2, bool add_carry) {
    int carry = add_carry ? state->cc.cy : 0;
    uint8_t value = (val1 & 0xF) + (val2 & 0xF) + carry;
    state->cc.ac = (value + carry) > 0xF;
}


static void _update_flag_and(State8080 *state, uint8_t res, uint8_t val1, uint8_t val2) {
    _update_flag_z(state, res);
    _update_flag_s(state, res);
    _update_flag_p(state, res);
    state->cc.cy = 0;
    state->cc.ac = ((val1 | val2) >> 3) & 1;
}

static void _update_flag_or(State8080  *state, int8_t res) {
    _update_flag_z(state, res);
    _update_flag_s(state, res);
    _update_flag_p(state, res);
    state->cc.cy = 0;
    state->cc.ac = 0;
} 

static void _swap(uint8_t *val1, uint8_t *val2) {
    uint8_t temp = *val1;

    *val1 = *val2;
    *val2 = temp;
}


//!================================= Arithmetic instructions: =================================//

void ADD_R(State8080 *state, REGISTERS reg) {
    uint8_t val1 = state->registers[A];
    uint8_t val2 = state->registers[reg];
    uint16_t answer = (uint16_t) val1 + (uint16_t) val2;

    _update_flag_z(state, answer);
    _update_flag_s(state, answer);
    _update_flag_cy_add(state, answer, false);
    _update_flag_p(state, answer);
    _update_flag_ac_add(state, val1, val2, false);

    state->registers[A] = answer & 0xFF;

}


void ADI(State8080 *state, uint8_t value) {

    uint16_t answer = (uint16_t) state->registers[A] + (uint16_t) value;

    _update_flag_z(state, answer);
    _update_flag_s(state, answer);
    _update_flag_p(state, answer);
    _update_flag_cy_add(state, answer, false);
    _update_flag_ac_add(state, state->registers[A], value, false); 
    
    state->registers[A] = answer & 0xFF;

}


void ADD_M(State8080 *state) {
    uint16_t offset = (state->registers[H] << 8) | (state->registers[L]);
    uint16_t answer = (uint16_t) state->registers[A] + state->memory[offset];
    
    _update_flag_z(state, answer);
    _update_flag_s(state, answer);
    _update_flag_cy_add(state, answer, false);
    _update_flag_p(state, answer);
    _update_flag_ac_add(state, state->registers[A], state->memory[offset], false);
    state->registers[A] = answer & 0xFF;

}


void ADC_R(State8080 *state, REGISTERS reg) {
    uint8_t val1 = state->registers[A];
    uint8_t val2 = state->registers[reg];
    uint8_t answer = val1 + val2 + state->cc.cy;

    _update_flag_z(state, answer);
    _update_flag_s(state, answer);
    _update_flag_p(state, answer);
    _update_flag_ac_add(state, val1, val2, true);
    _update_flag_cy_add(state, val1 + val2, true);

    state->registers[A] = answer;
}


void ADC_M(State8080 *state) {
    uint16_t offset = (state->registers[H] << 8) | (state->registers[L]);
    uint8_t val1 = state->registers[A];
    uint8_t val2 = state->memory[offset];
    uint8_t answer = val1 + val2 + state->cc.cy;

    _update_flag_z(state, answer);
    _update_flag_s(state, answer);
    _update_flag_p(state, answer);
    _update_flag_ac_add(state, val1, val2, true);
    _update_flag_cy_add(state, val1 + val2, true);

    state->registers[A] = answer;
}


void ACI(State8080 *state, uint8_t byte) {

    uint8_t val1 = state->registers[A];
    uint8_t answer = val1 + byte + state->cc.cy;

    _update_flag_z(state, answer);
    _update_flag_s(state, answer);
    _update_flag_p(state, answer);
    _update_flag_ac_add(state, val1, byte, true);
    _update_flag_cy_add(state, val1 + byte, true);

    state->registers[A] = answer;

    
}


void INR_R(State8080 *state, REGISTERS reg) {
    uint16_t answer = state->registers[reg] + 1;
    uint8_t value = state->registers[reg];
    _update_flag_z(state, answer);
    _update_flag_s(state, answer);
    _update_flag_p(state , answer);
    _update_flag_ac_add(state, value, 1, false);

    state->registers[reg] = answer;
}


void INR_M(State8080 *state) {
    uint16_t offset = (state->registers[H] << 8) | (state->registers[L]);
    uint8_t value = state->memory[offset];
    uint8_t answer = value + 1;

    _update_flag_z(state, answer);
    _update_flag_s(state, answer);
    _update_flag_p(state, answer);
    _update_flag_ac_add(state, value, 1, false);

    state->memory[offset] = answer;

}


void INX_PAIR(State8080 *state, REGISTERS reg) {
    uint16_t pair_value = (state->registers[reg] << 8) | state->registers[reg + 1];
    pair_value++;
    state->registers[reg] = (uint8_t) (pair_value >> 8);
    state->registers[reg + 1] = (uint8_t) (pair_value & 0xFF);
}


void INX_SP(State8080 *state) {
    state->sp += 1;
}


void SUB_R(State8080 *state, REGISTERS reg) {
    uint8_t val1 = state->registers[A];
    uint8_t val2 = state->registers[reg];
    uint8_t res = val1 - val2;
    _update_flag_z(state, res);
    _update_flag_s(state, res);
    _update_flag_p(state, res);
    _update_flag_cy_sub(state, val1, val2, false);
    _update_flag_ac_sub(state, val1, val2, false);

    state->registers[A] = res;
}


void SUB_M(State8080 *state) {
    uint16_t offset = (state->registers[H] << 8) | (state->registers[L]);
    uint8_t val1 = state->registers[A];
    uint8_t val2 = state->memory[offset];
    uint8_t res = val1 - val2;

    _update_flag_z(state, res);
    _update_flag_s(state, res);  
    _update_flag_p(state, res);
    _update_flag_cy_sub(state, val1, val2, false);
    _update_flag_ac_sub(state, val1, val2, false);

    state->registers[A] = res;
}


void SUI(State8080 *state, uint8_t byte) {

    uint8_t val = state->registers[A];
    uint8_t res = val - byte;

    _update_flag_z(state, res);
    _update_flag_s(state, res);  
    _update_flag_p(state, res);
    _update_flag_cy_sub(state, val, byte, false);
    _update_flag_ac_sub(state, val, byte, false);

    state->registers[A] = res;

}


void SBB_R(State8080 *state, REGISTERS reg) {
    uint8_t val1 = state->registers[A];
    uint8_t val2 = state->registers[reg];
    uint8_t answer = val1 - val2 - state->cc.cy;

    _update_flag_z(state, answer);
    _update_flag_s(state, answer);
    _update_flag_p(state, answer);
    _update_flag_ac_sub(state, val1, val2, true);
    _update_flag_cy_sub(state, val1, val2, true);

    state->registers[A] = answer;
}


void SBB_M(State8080 *state) {
    uint16_t offset = (state->registers[H] << 8) | (state->registers[L]);
    uint8_t val1 = state->registers[A];
    uint8_t val2 = state->memory[offset];
    uint8_t answer = val1 - val2 - state->cc.cy;

    _update_flag_z(state, answer);
    _update_flag_s(state, answer);
    _update_flag_p(state, answer);
    _update_flag_ac_sub(state, val1, val2, true);
    _update_flag_cy_sub(state, val1, val2, true);

    state->registers[A] = answer;
}


void SBI(State8080 *state, uint8_t byte) {

    uint8_t val = state->registers[A];
    uint8_t answer = val - byte - state->cc.cy;

    _update_flag_z(state, answer);
    _update_flag_s(state, answer);
    _update_flag_p(state, answer);
    _update_flag_ac_sub(state, val, byte, true);
    _update_flag_cy_sub(state, val, byte, true);

    state->registers[A] = answer;
    
}


void DCR_R(State8080 *state, REGISTERS reg) {
    uint8_t value = state->registers[reg];
    uint16_t result = state->registers[reg] - 1;

    _update_flag_z(state, result);
    _update_flag_s(state, result);
    _update_flag_p(state, result);
    _update_flag_ac_sub(state, value, 1, false);

    state->registers[reg] -= 1;
}


void DCR_M(State8080 *state) {
    uint16_t offset = (state->registers[H] << 8) | (state->registers[L]);
    uint8_t value = state->memory[offset];
    uint8_t answer = value - 1;

    _update_flag_z(state, answer);
    _update_flag_s(state, answer);  
    _update_flag_p(state, answer);
    _update_flag_ac_sub(state, value, 1, false);

    state->memory[offset] -= 1;

}


void DCX_PAIR(State8080 *state, REGISTERS reg) {
    uint16_t val = get_reg_pair(state, reg, reg + 1) - 1; 

    state->registers[reg] = (val >> 8);
    state->registers[reg + 1] = val & 0xFF;
}


void DCX_SP(State8080 *state) {
    state->sp -= 1;
}


void DAD_PAIR(State8080 *state, REGISTERS reg) {
    uint16_t pair_value1 = (state->registers[H] << 8) | state->registers[L];
    uint16_t pair_value2 = (state->registers[reg] << 8) | state->registers[reg + 1];

    uint32_t answer = pair_value1 + pair_value2;

    _update_flag_cy_add_16(state, answer);
    
    state->registers[H] = (uint8_t) ((answer >> 8) & 0xFF);
    state->registers[L] = (uint8_t) (answer & 0xFF);
}


void DAD_SP(State8080 *state) {
    uint16_t pair_value1 = (state->registers[H] << 8) | state->registers[L];

    uint32_t answer = pair_value1 + state->sp;
    _update_flag_cy_add_16(state, answer);

    state->registers[H] = (uint8_t) ((answer >> 8) & 0xFF);
    state->registers[L] = (uint8_t) (answer & 0xFF);
} 


void DAA(State8080 *state) {
    uint8_t acc_val = state->registers[A];
    uint8_t val = 0;
    if (((state->registers[A] & 0x0F) > 0x09) || state->cc.ac) {
        val += 0x06;
        state->registers[A] += 0x06;
    }

    if (((state->registers[A] >> 4) > 0x09) || state->cc.cy) {
        val += 0x60;
        state->registers[A] += 0x60;
        state->cc.cy = 1;
    }
    uint8_t res = state->registers[A];

    _update_flag_z(state, res);
    _update_flag_s(state, res);
    _update_flag_p(state, res);
    _update_flag_ac_add(state, acc_val, val, false);

}

//!================================= Branch instructions: =================================//

void CALL(State8080* state, uint8_t byte1, uint8_t byte2) {

    state->memory[(uint16_t)(state->sp - 1)] = state->pc >> 8;
    state->memory[(uint16_t)(state->sp - 2)] = state->pc & 0xFF;
    
    state->sp -= 2;
    state->pc = (byte2 << 8) | byte1;
}



void CZ(State8080 *state, uint8_t byte1, uint8_t byte2) {

    if (state->cc.z == 1) {
        CALL(state, byte1, byte2);  
    }
}


void CNZ(State8080 *state, uint8_t byte1, uint8_t byte2) {

    if (state->cc.z == 0) {
        CALL(state, byte1, byte2);  
    }
}


void CC(State8080 *state, uint8_t byte1, uint8_t byte2) {

    if (state->cc.cy == 1) {
        CALL(state, byte1, byte2);  
    }
}


void CNC(State8080 *state, uint8_t byte1, uint8_t byte2) {

    if (state->cc.cy == 0) {
        CALL(state, byte1, byte2);  
    }
}


void CPO(State8080 *state, uint8_t byte1, uint8_t byte2) {

    if (state->cc.p == 0) {
        CALL(state, byte1, byte2);  
    }
}


void CPE(State8080 *state, uint8_t byte1, uint8_t byte2) {

    if (state->cc.p == 1) {
        CALL(state, byte1, byte2);  
    }
}


void CP(State8080 *state, uint8_t byte1, uint8_t byte2) {

    if (state->cc.s == 0) {
        CALL(state, byte1, byte2);  
    }
}


void CM(State8080 *state, uint8_t byte1, uint8_t byte2) {

    if (state->cc.s == 1) {
        CALL(state, byte1, byte2);  
    }
}


void RET(State8080 *state) {
    state->pc = (state->memory[(uint16_t)(state->sp + 1)] << 8) | state->memory[state->sp];
    state->sp += 2; 
}


void RNZ(State8080 *state) {
    if (state->cc.z == 0) {
        RET(state);
    }
}


void RZ(State8080 *state) {
    if (state->cc.z == 1) {
        RET(state);
    }
}


void RNC(State8080 *state) {
    if (state->cc.cy == 0) {
        RET(state);
    }
}


void RC(State8080 *state) {
    if (state->cc.cy == 1) {
        RET(state);
    }
}


void RPO(State8080 *state) {
    if (state->cc.p == 0) {
        RET(state);
    }
}


void RPE(State8080 *state) {
    if (state->cc.p == 1) {
        RET(state);
    }
}


void RP(State8080 *state) {
    if (state->cc.s == 0) {
        RET(state);
    }
}


void RM(State8080 *state) {
    if (state->cc.s == 1) {
        RET(state);
    }
}


void RST_N(State8080 *state, int n) {
    state->memory[(uint16_t)(state->sp - 1)] = state->pc >> 8;
    state->memory[(uint16_t)(state->sp - 2)] = state->pc & 0xFF;

    state->sp -= 2;
    state->pc = 8 * n;
}


void PCHL(State8080 *state) {
    state->pc = ((state->registers[H]) << 8) | (state->registers[L]);
}


void JMP(State8080 *state, uint8_t byte1, uint8_t byte2) {
    state->pc = (byte2 << 8) | (byte1);
}


void JNZ(State8080 *state, uint8_t byte1, uint8_t byte2) {

    if (state->cc.z == 0) {
        JMP(state, byte1, byte2);
    }
}


void JZ(State8080 *state, uint8_t byte1, uint8_t byte2) {

    if (state->cc.z == 1) {
        JMP(state, byte1, byte2);
    }
}


void JC(State8080 *state, uint8_t byte1, uint8_t byte2) {

    if (state->cc.cy == 1) {
        JMP(state, byte1, byte2);
    }
}


void JNC(State8080 *state, uint8_t byte1, uint8_t byte2) {

    if (state->cc.cy == 0) {
        JMP(state, byte1, byte2);
    }
}


void JM(State8080 *state, uint8_t byte1, uint8_t byte2) {

    if (state->cc.s == 1) {
        JMP(state, byte1, byte2);
    }
}


void JP(State8080 *state, uint8_t byte1, uint8_t byte2) {

    if (state->cc.s == 0) {
        JMP(state, byte1, byte2);
    }
}


void JPE(State8080 *state, uint8_t byte1, uint8_t byte2) {

    if (state->cc.p == 1) {
        JMP(state, byte1, byte2);
    }
}


void JPO(State8080 *state, uint8_t byte1, uint8_t byte2) {

    if (state->cc.p == 0) {
        JMP(state, byte1, byte2);
    }
}




//!=================================Stack and I/O instructions: =================================//


void NOP() {
    // Nothing happens
}


void PUSH(State8080 *state, REGISTERS reg) {  // Use PUSH(state, H) for PUSH M 
    state->memory[(uint16_t)(state->sp - 1)] = state->registers[reg];
    state->memory[(uint16_t)(state->sp - 2)] = state->registers[reg + 1];

    state->sp -= 2;
}


void PUSH_PSW(State8080 *state) {
    state->memory[(uint16_t)(state->sp - 1)] = state->registers[A];

    uint8_t psw = (state->cc.s << 7) |
                  (state->cc.z << 6) |
                  (state->cc.ac << 4) |
                  (state->cc.p << 2) |
                  (1 << 1) |
                  state->cc.cy;
            
    state->memory[(uint16_t)(state->sp - 2)] = psw;

    state->sp -= 2;
} 


void POP(State8080 *state, REGISTERS reg) {
    state->registers[reg + 1] = state->memory[state->sp];
    state->registers[reg] = state->memory[(uint16_t)(state->sp + 1)];

    state->sp += 2;
}


void POP_PSW(State8080 *state) {
    uint8_t psw = state->memory[state->sp];

    state->cc.cy = (int8_t)(psw & 1);
    state->cc.p = (int8_t)((psw >> 2) & 1);
    state->cc.ac = (int8_t)((psw >> 4) & 1);
    state->cc.z = (int8_t)((psw >> 6) & 1);
    state->cc.s = (int8_t)((psw >> 7) & 1);

    state->registers[A] = state->memory[(uint16_t)(state->sp + 1)];

    state->sp += 2;
}


void XTHL(State8080 *state) {
    _swap(&state->registers[L], &state->memory[state->sp]);
    _swap(&state->registers[H], &state->memory[(uint16_t)(state->sp + 1)]);
}


void SPHL(State8080 *state) {
    state->sp = (state->registers[H] << 8) | (state->registers[L]);
}


void IN(State8080 *state, uint8_t port) {
    state->registers[A] = state->input[port]();
}


void OUT(State8080 *state, uint8_t port) {
    state->output[port](state->registers[A]);
}


void EI(State8080 *state) {
    state->int_enable = 1;
}


void DI(State8080 *state) {
    state->int_enable = 0;
}


void HLT(State8080 *state) {
    state->halt = true;
}


void XT(State8080 *state) {
    state->exit = true;
}

//!================================= Data Transfer instructions: =================================//

void STA(State8080 *state, uint8_t byte1, uint8_t byte2) {

    uint16_t address = ((uint16_t) byte2 << 8) | (byte1);
    state->memory[address] = state->registers[A];

}


void MOV_R_R(State8080 *state, REGISTERS reg1, REGISTERS reg2) {
    state->registers[reg1] = state->registers[reg2];
}


void MOV_R_M(State8080 *state, REGISTERS reg) {
    uint16_t offset = (state->registers[H] << 8) | (state->registers[L]);
    state->registers[reg] = state->memory[offset];
}


void MOV_M_R(State8080 *state, REGISTERS reg) {
    uint16_t offset = (state->registers[H] << 8) | (state->registers[L]);
    state->memory[offset] = state->registers[reg];
}


void MVI_R(State8080 *state, REGISTERS reg, uint8_t byte) {
 
    state->registers[reg] = byte;
}


void MVI_M(State8080 *state, uint8_t byte) {

    uint16_t offset = (state->registers[H] << 8) | (state->registers[L]);
    state->memory[offset] = byte;
}


void LXI_PAIR(State8080 *state, REGISTERS reg, uint8_t byte1, uint8_t byte2) {

    _cpu_set_reg_pair(state, reg, reg + 1, byte1, byte2);
}


void LXI_SP(State8080 *state, uint8_t byte1, uint8_t byte2) {

    state->sp = (byte2 << 8) | byte1;
}


void LDA(State8080 *state, uint8_t byte1, uint8_t byte2) {

    uint16_t offset = (byte2 << 8) | byte1;
    state->registers[A] = state->memory[offset];
}


void LDAX(State8080 *state, REGISTERS reg) {
    uint16_t offset = (state->registers[reg] << 8) | state->registers[reg + 1];
    state->registers[A] = state->memory[offset];
}


void SHLD(State8080 *state, uint8_t byte1, uint8_t byte2) {

    uint16_t offset = (byte2 << 8) | byte1;
    state->memory[offset] = state->registers[L];
    state->memory[(uint16_t)(offset + 1)] = state->registers[H];
}


void LHLD(State8080 *state, uint8_t byte1, uint8_t byte2) {

    uint16_t offset = (byte2 << 8) | byte1;
    state->registers[H] = state->memory[(uint16_t)(offset + 1)];
    state->registers[L] = state->memory[offset];
}


void STAX(State8080 *state, REGISTERS reg) {
    uint16_t offset = (state->registers[reg] << 8) | state->registers[reg + 1];
    state->memory[offset] = state->registers[A];
}


void XCHG(State8080 *state) {
    _swap(&state->registers[H], &state->registers[D]);
    _swap(&state->registers[L], &state->registers[E]);
}

//!================================= Logical instructions: =================================//

void ANA_R(State8080 *state, REGISTERS reg) {
    uint8_t val1 = state->registers[A];
    uint8_t val2 = state->registers[reg];
    uint8_t res = val1 & val2;

    _update_flag_and(state, res, val1, val2);

    state->registers[A] = res;
}


void ANA_M(State8080 *state) {
    uint16_t offset = (state->registers[H] << 8) | (state->registers[L]);
    uint8_t val1 = state->registers[A];
    uint8_t val2 = state->memory[offset];
    uint8_t res = val1 & val2;

    _update_flag_and(state, res, val1, val2);
    state->registers[A] = res;
}


void ANI(State8080 *state, uint8_t byte) {

    uint8_t val = state->registers[A];
    uint8_t res = val & byte;

    _update_flag_and(state, res, val, byte);

    state->registers[A] = res;   
}


void XRA_R(State8080 *state, REGISTERS reg) {
    uint8_t res = (state->registers[A]) ^ (state->registers[reg]);
    _update_flag_or(state, res);

    state->registers[A] = res;
}


void XRA_M(State8080 *state) {
    uint16_t offset = (state->registers[H] << 8) | (state->registers[L]);
    uint8_t res = (state->registers[A]) ^ (state->memory[offset]);
    _update_flag_or(state, res);

    state->registers[A] = res;
}


void XRI(State8080 *state, uint8_t byte) {

    uint8_t res = (state->registers[A]) ^ byte;
    _update_flag_or(state, res);

    state->registers[A] = res;
}


void ORA_R(State8080 *state, REGISTERS reg) {
    uint8_t res = (state->registers[A]) | (state->registers[reg]);
    
    _update_flag_or(state, res);

    state->registers[A] = res;
}


void ORA_M(State8080 *state) {
    uint16_t offset = (state->registers[H] << 8) | (state->registers[L]);
    uint8_t res = (state->registers[A]) | (state->memory[offset]);
    _update_flag_or(state, res);

    state->registers[A] = res;
}


void ORI(State8080 *state, uint8_t byte) {

    uint8_t res = (state->registers[A]) | byte;
    _update_flag_or(state, res);

    state->registers[A] = res;
    
}


void CMP_R(State8080 *state, REGISTERS reg) {
    uint8_t res = state->registers[A] - state->registers[reg];
    uint8_t val1 = state->registers[A];
    uint8_t val2 = state->registers[reg];
    _update_flag_z(state, res);
    _update_flag_s(state, res);
    _update_flag_p(state, res);
    _update_flag_cy_sub(state, val1, val2, false);
    _update_flag_ac_sub(state, val1, val2, false);
}


void CMP_M(State8080 *state) {
    uint16_t offset = (state->registers[H] << 8) | (state->registers[L]);
    uint8_t res = state->registers[A] - state->memory[offset];
    uint8_t val1 = state->registers[A];
    uint8_t val2 = state->memory[offset];
    _update_flag_z(state, res);
    _update_flag_s(state, res);
    _update_flag_p(state, res);
    _update_flag_cy_sub(state, val1, val2, false);
    _update_flag_ac_sub(state, val1, val2, false);

}


void CPI(State8080 *state, uint8_t byte) {

    
    uint8_t res = state->registers[A] - byte;
    uint8_t val = state->registers[A];
    _update_flag_z(state, res);
    _update_flag_s(state, res);
    _update_flag_p(state, res);
    _update_flag_cy_sub(state, val, byte, false);
    _update_flag_ac_sub(state, val, byte, false);

}


void CMA(State8080 *state) {
    state->registers[A] = ~(state->registers[A]);
}


void CMC(State8080 *state) {
    state->cc.cy = ~(state->cc.cy);
}


void STC(State8080 *state) {
    state->cc.cy = 1;
}


//!================================= Rotate instructions: =================================//

void RRC(State8080 *state) {
    uint8_t acc_val = state->registers[A];
    uint8_t lsb = acc_val & 1;
    acc_val = (acc_val >> 1) | (lsb << 7);
    
    state->cc.cy = lsb;
    state->registers[A] = acc_val;
}


void RLC(State8080 *state) {
    uint8_t acc_val = state->registers[A];
    uint8_t msb = acc_val >> 7;
    acc_val = (acc_val << 1) | msb;

    state->cc.cy = msb;
    state->registers[A] = acc_val;
}


void RAR(State8080 *state) {
    uint8_t acc_val = state->registers[A];
    uint8_t lsb = acc_val & 1;
    acc_val = (acc_val >> 1) | (state->cc.cy << 7);

    state->cc.cy = lsb;
    state->registers[A] = acc_val;
}


void RAL(State8080 *state) {
    uint8_t acc_val = state->registers[A];
    uint8_t msb = acc_val >> 7;
    acc_val = (acc_val << 1) | state->cc.cy;

    state->cc.cy = msb;
    state->registers[A] = acc_val;
}