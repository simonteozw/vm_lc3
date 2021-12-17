#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/termios.h>
#include <signal.h>

// LC-3 has 2^16 memory locations
uint16_t memory[UINT16_MAX];

// 8 general purpose registers, 1 PC, 1 conditional flag
enum {
    R_R0 = 0,
    R_R1,
    R_R2,
    R_R3,
    R_R4,
    R_R5,
    R_R6,
    R_R7,
    R_PC,
    R_COND,
    R_COUNT
};

uint16_t reg[R_COUNT];

enum {
    OP_BR = 0, // branch
    OP_ADD,    // add
    OP_LD,     // load
    OP_ST,     // store
    OP_JSR,    // jump register
    OP_AND,    // bitwise and
    OP_LDR,    // load register
    OP_STR,    // store register
    OP_RTI,    // unused
    OP_NOT,    // bitwise not
    OP_LDI,    // load indirect
    OP_STI,    // store indirect
    OP_JMP,    // jump
    OP_RES,    // unused
    OP_LEA,    // load effective address
    OP_TRAP    // execute trap
};

enum {
    FL_POS = 1 << 0, // positive
    FL_ZRO = 1 << 1, // zero
    FL_NEG = 1 << 2  // negative
};

uint16_t sign_extend(uint16_t x, int bitcount) {
    // if x is negative add ones (using 2s complement)
    if ((x >> (bitcount - 1)) & 1) x |= (0xFFFF << bitcount);
    return x;
}

void update_flags(uint16_t r) {
    if (reg[r] == 0) reg[R_COND] = FL_ZRO;
    else if (reg[r] >> 15) reg[R_COND] = FL_NEG; // 1 in left most bit means -ve
    else reg[R_COND] = FL_POS;
}

int main(int argc, const char* argv[]) {

    if (argc < 2) {
        /* show usage string */
        printf("./main [image-file1] ...\n");
        exit(2);
    }
    for (int j = 1; j < argc; j++) {
        if (!read_image(argv[j])) {
            printf("failed to load image: %s\n", argv[j]);
            exit(1);
        }
    }

    enum { PC_START = 0x3000 };
    reg[R_PC] = PC_START;

    int running = 1;
    while (running) {
        uint16_t instr = mem_read(reg[R_PC]);
        reg[R_PC]++;
        uint16_t op = instr >> 12;

        switch (op) {
            case OP_BR:
            case OP_ADD:
            case OP_LD:
            case OP_ST:
            case OP_JSR:
            case OP_AND:
            case OP_LDR:
            case OP_STR:
            case OP_RTI:
            case OP_NOT:
            case OP_LDI:
            case OP_STI:
            case OP_JMP:
            case OP_RES:
            case OP_LEA:
            case OP_TRAP:
            default:
                break;
        }
    }
}