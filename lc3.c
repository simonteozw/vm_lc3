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

enum {
    MR_KBSR = 0xFE00, // keyboard status
    MR_KBDR = 0xFE02  // keyboard data
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

uint16_t check_key()
{
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    return select(1, &readfds, NULL, NULL, &timeout) != 0;
}

void mem_write(uint16_t address, uint16_t val) {
    memory[address] = val;
}

uint16_t mem_read(uint16_t address) {
    if (address == MR_KBSR) {
        if (check_key()) {
            memory[MR_KBSR] = (1 << 15);
            memory[MR_KBDR] = getchar();
        } else {
            memory[MR_KBSR] = 0;
        }
    }
    return memory[address];
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
            {
                uint16_t nzp = (instr >> 9) & 0x7;
                if (nzp & reg[R_COND]) {
                    reg[R_PC] += sign_extend(instr & 0x1FF, 9); // pc offset
                }
            }
            break;
            case OP_ADD:
            {
                uint16_t dr = (instr >> 9) & 0x7;
                uint16_t sr1 = (instr >> 6) & 0x7;
                uint16_t imm_flag = (instr >> 5) & 0x1;

                if (imm_flag) {
                    uint16_t imm = sign_extend(instr & 0x1F, 5);
                    reg[dr] = reg[sr1] + imm;
                } else {
                    uint16_t sr2 = instr & 0x7;
                    reg[dr] = reg[sr1] + reg[sr2];
                }

                update_flags(dr);
            }
            break;
            case OP_LD:
            {
                uint16_t dr = (instr >> 9) & 0x7;
                uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
                reg[dr] = mem_read(reg[R_PC] + pc_offset);
                update_flags(dr);
            }
            break;
            case OP_ST:
            {
                uint16_t sr = (instr >> 9) & 0x7;
                uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
                mem_write(reg[R_PC] + pc_offset, reg[sr]);
            }
            break;
            case OP_JSR:
            {
                uint16_t long_flag = (instr >> 11) & 0x1;
                reg[R_R7] = reg[R_PC];
                if (long_flag) {
                    uint16_t long_pc_offset = sign_extend(instr & 0x7FF, 11);
                    reg[R_PC] += long_pc_offset; // JSR
                } else {
                    uint16_t base_r = (instr >> 6) & 0x7;
                    reg[R_PC] = reg[base_r]; // JSRR
                }
            }
            break;
            case OP_AND:
            {
                uint16_t dr = (instr >> 9) & 0x7;
                uint16_t sr1 = (instr >> 6) & 0x7;
                uint16_t imm_flag = (instr >> 5) & 0x1;

                if (imm_flag) {
                    uint16_t sr2 = instr & 0x7;
                    reg[dr] = reg[sr1] & reg[sr2];
                } else {
                    uint16_t imm = sign_extend(instr & 0x1F, 5);
                    reg[dr] = reg[sr1] & imm;
                }

                update_flags(dr);
            }
            break;
            case OP_LDR:
            {
                uint16_t dr = (instr >> 9) & 0x7;
                uint16_t base_r = (instr >> 6) & 0x7;
                uint16_t pc_offset = sign_extend(instr & 0x3F, 6);

                reg[dr] = mem_read(reg[base_r] + pc_offset);

                update_flags(dr);
            }
            break;
            case OP_STR:
            {
                uint16_t sr = (instr >> 9) & 0x7;
                uint16_t base_r = (instr >> 6) & 0x7;
                uint16_t pc_offset = sign_extend(instr & 0x3F, 6);

                mem_write(reg[base_r] + pc_offset, reg[sr]);
            }
            break;
            case OP_NOT:
            {
                uint16_t dr = (instr >> 9) & 0x7;
                uint16_t sr = (instr >> 6) & 0x7;

                reg[dr] = ~reg[sr];

                update_flags(dr);
            }
            break;
            case OP_LDI:
            {
                uint16_t dr = (instr >> 9) & 0x7;
                uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);

                reg[dr] = mem_read(mem_read(reg[R_PC] + pc_offset));

                update_flags(dr);
            }
            break;
            case OP_STI:
            case OP_JMP:
            case OP_LEA:
            case OP_TRAP:
            case OP_RTI:
            case OP_RES:
            default:
            break;
        }
    }
}