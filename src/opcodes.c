#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "emu8080.h"

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

static void _update_flag_cy(State8080 *state, uint16_t res) {
    state->cc.cy = res > 0xFF;
}

static void _update_flag_p(State8080 *state, uint16_t res) {
    state->cc.p = Parity(res & 0xFF);
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



void ADD_R(State8080 *state, REGISTERS reg) {
    uint8_t val1 = state->registers[A];
    uint8_t val2 = state->registers[reg];
    uint16_t answer = (uint16_t) val1 + (uint16_t) val2;

    _update_flag_z(state, answer);
    _update_flag_s(state, answer);
    _update_flag_cy(state, answer);
    _update_flag_p(state, answer);
    _update_flag_ac_add(state, val1, val2, false);

    state->registers[A] = answer & 0xFF;

}


void STA(State8080 *state, uint8_t byte1, uint8_t byte2) {
    uint16_t address = ((uint16_t) byte2 << 8) | (byte1);
    state->memory[address] = state->registers[A];

    state->pc += 2;
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


void ADD_I(State8080 *state, uint8_t value) {
    uint16_t answer = (uint16_t) state->registers[A] + (uint16_t) value;

    _update_flag_z(state, answer);
    _update_flag_s(state, answer);
    _update_flag_cy(state, answer);
    _update_flag_p(state, answer);
    _update_flag_ac_add(state, state->registers[A], value, false); 
    
    state->registers[A] = answer & 0xFF;

}


void ADD_M(State8080 *state) {
    uint16_t offset = (state->registers[H] << 8) | (state->registers[L]);
    uint16_t answer = (uint16_t) state->registers[A] + state->memory[offset];
    
    _update_flag_z(state, answer);
    _update_flag_s(state, answer);
    _update_flag_cy(state, answer);
    _update_flag_p(state, answer);
    _update_flag_ac_add(state, state->registers[A], state->memory[offset], false);
    state->registers[A] = answer & 0xFF;

}


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
    if (state->cc.z != 0) {
        state->pc = (byte2 << 8) | (byte1);
    }
    else {
        state->pc += 2;
    }
}


void PUSH_R(State8080 *state, REGISTERS src) {
    state->memory[state->sp-1] = state->registers[src];
    state->memory[state->sp-2] = state->registers[src + 1];

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

