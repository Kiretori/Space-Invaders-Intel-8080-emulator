#ifndef EMU8080_H
#define EMU8080_H

#include <stdint.h>
#include <stdbool.h>

#define REG_NUMBER 7
#define MAX_MEM 0x10000
#define NUM_IO 0xFF
#define NUM_OPCODES 0x100


typedef uint8_t (*input_ptr)(void);
typedef void (*output_ptr)(uint8_t);

typedef struct ConditionCodes {
	uint8_t z:1;
	uint8_t s:1;
	uint8_t p:1;
	uint8_t cy:1;
	uint8_t ac:1;
	uint8_t pad:3;
} ConditionCodes;

typedef struct State8080 {
	uint8_t registers[REG_NUMBER];
	uint16_t sp;
	uint16_t pc;
	uint8_t memory[MAX_MEM];
	ConditionCodes cc;
	uint8_t int_enable:1;

	input_ptr input[NUM_IO];
	output_ptr output[NUM_IO];

	bool halt;
	bool exit;

	unsigned long total_cycles;

} State8080;

typedef enum REGISTERS {
    B,
    C,
    D,
    E,
    H,
    L,
    A
} REGISTERS;

void Reset8080(State8080 *state);

void Emulate8080Op(State8080 *state);

int Disassemble8080Op(unsigned char *codebuffer, int pc);

void UnimplimentedInstruction(State8080 *state);

#endif