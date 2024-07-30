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
    state->cc.ac = ((val1 & 0xF) + (val2 & 0xF) + carry) > 0xF;
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


void ADD_I(State8080 *state, uint8_t value) {
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


void INX_SP(State8080 *state, REGISTERS reg) {
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
    uint8_t val1 = state->memory[A];
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
    uint16_t pair_value = (state->registers[reg] << 8) | state->registers[reg + 1];
    pair_value--;
    
}


void DCX_SP(State8080 *state) {
    state->sp -= 1;
}


void DAD(State8080 *state, REGISTERS reg) {
    uint16_t pair_value1 = (state->registers[H] << 8) | state->registers[L];
    uint16_t pair_value2 = (state->registers[reg] << 8) | state->registers[reg + 1];

    uint32_t answer = pair_value1 + pair_value2;

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
    uint16_t return_adr = state->pc + 2;
    state->memory[state->sp - 1] = return_adr >> 8;
    state->memory[state->sp - 2] = return_adr & 0xFF;
    state->sp -= 2;
    state->pc = (byte2 << 8) | byte1;
}


void JMP(State8080 *state, uint8_t byte1, uint8_t byte2) {
    state->pc = (byte2 << 8) | (byte1);
}


void JNZ(State8080 *state, uint8_t byte1, uint8_t byte2) {
    if (state->cc.z == 0) {
        state->pc = (byte2 << 8) | (byte1);
    }
    else {
        state->pc += 2;
    }
}


void JZ(State8080 *state, uint8_t byte1, uint8_t byte2) {
    if (state->cc.z == 1) {
        state->pc = (byte2 << 8) | (byte1);
    }
    else {
        state->pc += 2;
    }
}


void JC(State8080 *state, uint8_t byte1, uint8_t byte2) {
    if (state->cc.cy == 1) {
        state->pc = (byte2 << 8) | (byte1);
    }
    else {
        state->pc += 2;
    }
}


void JNC(State8080 *state, uint8_t byte1, uint8_t byte2) {
    if (state->cc.cy == 0) {
        state->pc = (byte2 << 8) | (byte1);
    }
    else {
        state->pc += 2;
    }
}


void JM(State8080 *state, uint8_t byte1, uint8_t byte2) {
    if (state->cc.s == 1) {
        state->pc = (byte2 << 8) | (byte1);
    }
    else {
        state->pc += 2;
    }
}


void JP(State8080 *state, uint8_t byte1, uint8_t byte2) {
    if (state->cc.s == 0) {
        state->pc = (byte2 << 8) | (byte1);
    }
    else {
        state->pc += 2;
    }
}


void JPE(State8080 *state, uint8_t byte1, uint8_t byte2) {
    if (state->cc.p == 1) {
        state->pc = (byte2 << 8) | (byte1);
    }
    else {
        state->pc += 2;
    }
}


void JPO(State8080 *state, uint8_t byte1, uint8_t byte2) {
    if (state->cc.p == 0) {
        state->pc = (byte2 << 8) | (byte1);
    }
    else {
        state->pc += 2;
    }
}


//!=================================Stack and I/O instructions: =================================//

void PUSH(State8080 *state, REGISTERS src) {  // Use PUSH(state, H) for PUSH M 
    state->memory[state->sp - 1] = state->registers[src];
    state->memory[state->sp - 2] = state->registers[src + 1];

    state->sp -= 2;
}


void PUSH_PSW(State8080 *state) {
    state->memory[state->sp-1] = state->registers[A];

    uint8_t psw = (state->cc.s << 7) |
                  (state->cc.z << 6) |
                  (state->cc.ac << 4) |
                  (state->cc.p << 2) |
                  (1 << 1) |
                  state->cc.cy;
            
    state->memory[state->sp-2] = psw;

    state->sp -= 2;
} 

//!================================= Data Transfer instructions: =================================//

void STA(State8080 *state, uint8_t byte1, uint8_t byte2) {
    uint16_t address = ((uint16_t) byte2 << 8) | (byte1);
    state->memory[address] = state->registers[A];

    state->pc += 2;
}


void MVI_R(State8080 *state, REGISTERS reg, uint8_t byte) {
    state->registers[reg] = byte;

    state->pc += 1;   
}


void MVI_M(State8080 *state, uint8_t byte) {
    uint16_t offset = (state->registers[H] << 8) | (state->registers[L]);
    state->memory[offset] = byte;
    state->pc += 1;
}


void LXI_PAIR(State8080 *state, REGISTERS reg1, REGISTERS reg2, uint8_t byte1, uint8_t byte2) {
    _cpu_set_reg_pair(state, reg1, reg2, byte1, byte2);
    state->pc += 2;
}


void LXI_SP(State8080 *state, uint8_t byte1, uint8_t byte2) {
    state->sp = (byte2 << 8) | byte1;
    state->pc += 2;
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

    state->pc += 1; 
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

    state->pc += 1;
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
    uint8_t res = (state->registers[A]) ^ byte;
    _update_flag_or(state, res);

    state->registers[A] = res;

    state->pc += 1;
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

    state->pc += 1;
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
