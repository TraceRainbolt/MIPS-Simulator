/*----------------------------------------------------------------------*
 *  Example mips_asm program loader. This loads the mips_asm binary *
 *  named "testcase1.mb" into an array in memory. It reads      *
 *  the 64-byte header, then loads the code into the mem array. *
 *                                  *
 *  DLR 4/18/16                         *
 *----------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "loader.h"

#include "mips_asm_header.h"

#define NUM_REGS 32

#define V0 2
#define A0 4

/* Create memory and registers */
MIPS mem[MAX_MEM_ADDR];
MIPS R[NUM_REGS];
MIPS pc = 0;

int num_instructions = 0;
int num_cycles = 0;
int num_memref = 0;
void initRegisters(MIPS registers[]){
    int i;
    for(i = 0; i < NUM_REGS; i++)
        registers[i] = 0;
}

void printRegisters() {
    int i;
    char reg[8];
    printf("Number of Instructions Executed: %d\n", num_instructions);
    printf("Number of Clock Cycles: %d\n", num_cycles);
    printf("Number of Memory References: %d\n", num_memref);
    for(i = 0; i < NUM_REGS; i++) {
        getRegString(i,reg);
        printf("%s: 0x%08X\n", reg, R[i]);
    }
    printf("PC: 0x%08X\n", 0x400000 + pc);
}

int arithShift(int x, unsigned k) {
    if ( x >= 0 ) {
        return x / (1 << k);
    } 
    else {
        return x / (1 << k) - ( x % (1 << k) == 0 ? 0 : 1 );
    }
}

void handleSyscall(Instruction inst){
    int call_code = R[V0];
    char buffer[256];
    switch(call_code){
        case 1:
            printf("%d", R[A0]);
            break;
        case 4:
            printf("%s", (char *) (long) R[A0]);
            break;
        case 5:
            scanf("%d", &R[V0]);
            break;
        case 8:
            scanf("%s", buffer);
            break;
        case 10:
            printf("\n");
            printRegisters();
            exit(0);
        default:
            printf("Error: bad syscall");
            exit(0);
    }
}

void executeFunction(Instruction inst){
    switch(inst.function) {
        case SLL:
            R[inst.rd] = R[inst.rt] << inst.shamt;
            break;
        case SRL:
            R[inst.rd] = R[inst.rt] >> inst.shamt;
            break;
        case SRA:
            R[inst.rd] = arithShift(R[inst.rt], inst.shamt);
            break;
        case SLLV:
            R[inst.rd] = R[inst.rt] << R[inst.rs];
            break;
        case SRLV:
            R[inst.rd] = R[inst.rt] >> R[inst.rs]; 
            break;
        case SRAV:
            R[inst.rd] = arithShift(R[inst.rt], R[inst.rs]);
            break;
        case JR:
            pc = R[inst.rs];
            break;
        case JALR:
            pc = R[inst.rs];
            R[31] = pc; 
            break;
        case ADD:
            R[inst.rd] = (int) R[inst.rs] + (int) R[inst.rt];
            break;
        case ADDU:
            R[inst.rd] = R[inst.rs] + R[inst.rt];
            break;
        case SUB:
            R[inst.rd] = (int) R[inst.rs] - (int) R[inst.rt];
            break;
        case SUBU:
            R[inst.rd] = R[inst.rs] - R[inst.rt];
            break;
        case AND:
            R[inst.rd] = R[inst.rs] & R[inst.rt];
            break;
        case OR:
            R[inst.rd] = R[inst.rs] | R[inst.rt];
            break;
        case XOR:
            R[inst.rd] = R[inst.rs] ^ R[inst.rt];
            break;
        case NOR:
            R[inst.rd] = ~(R[inst.rs] | R[inst.rt]);
            break;
        case SLT:
            R[inst.rd] = (R[inst.rs] < R[inst.rt]) ? 1 : 0;
            break;
        case SLTU:
            R[inst.rd] = (R[inst.rs] < R[inst.rt]) ? 1 : 0;
            break;
        case SYSCALL:
            handleSyscall(inst);
            break;
    }
}

void execute(Instruction inst){
    switch(inst.opcode) {
        /* Gotta function code to handle */
        case 0x00:
            executeFunction(inst);
            break;
        case J:
            pc = inst.jump_addr;
            break;
        case JAL:
            R[31] = pc;
            pc = inst.jump_addr;
            break;
        case BEQ:
            if (R[inst.rs] == R[inst.rt]) pc = pc + (inst.signext << 2);
            break;
        case BNE:
            if (R[inst.rs] != R[inst.rt]) pc = pc + (inst.signext << 2);
            break;
        case ADDI:
            R[inst.rt] = R[inst.rs] + inst.imm;
            break;
        case SLTI:
            R[inst.rt] = (R[inst.rs] < inst.signext) ? 1 : 0;
            break;
        case SLTIU:
            R[inst.rt] = (R[inst.rs]< inst.signext) ? 1 : 0;
            break;
        case ANDI:
            R[inst.rt] = R[inst.rs] + inst.signext;
            break;
        case ORI:
            R[inst.rt] = R[inst.rs] | inst.imm;
            break;
        case XORI:
            R[inst.rt] = R[inst.rs] ^ inst.imm;
            break;
        case LUI:
            R[inst.rt] = inst.imm << 16;
            break;
        case LB:
            R[inst.rt] = (char) (mem[(R[inst.rs]) / 4] + inst.imm);
            break;
        case LH:
            R[inst.rt] = (short) (mem[(R[inst.rs]) / 4] + inst.imm);
            break;
        case LW:
            R[inst.rt] = mem[(R[inst.rs] + inst.signext) / 4];
            break;
        case LBU:
            R[inst.rt] = (unsigned char) (mem[(R[inst.rs]) / 4] + inst.imm);
            break;
        case LHU:
            R[inst.rt] = (unsigned short) (mem[(R[inst.rs]) / 4] + inst.imm);
            break;
        case SB:
            mem[R[inst.rs] + inst.imm] = (char)R[inst.rt];
            break;
        case SH:
            mem[R[inst.rs] + inst.imm] = (short)R[inst.rt];
            break;
        case SW:
            mem[R[inst.rs] + inst.imm] = R[inst.rt];
            break;
    }
}

int main(){
    /* File to be ran */
    char filename[] = "countbits.mb";
    initRegisters(R);
    int input;
    int memp = loadProgram(filename, mem);


    /* Select Running Mode */
    printf("Select Running Mode:\n");
    printf("\tEnter '1' for simulation mode.\n");
    printf("\tEnter '2' for step mode.\n");
    printf("\tEnter '3' to exit.\n");


    scanf("%i", &input);
    fflush(stdin);



    if(input == 3) {
        exit(0);
    }

    while(pc < memp){
        if(input == 2) {
            printf("Press enter to step through the function or q to exit.\n\n");
            if(getchar() == "q") {
                exit(0);
            }
            fflush(stdin);
        }
        int raw_hex = mem[pc / 4];
        Instruction instruction;
        char name[8];

        fetchAndDecode(&instruction, raw_hex);

        getOpName(raw_hex, name);
        if(name[0] == 'b' || strcmp(name, "nop") == 0 || name[0] == 'j')
            num_cycles += 3;
        else if(name[0] == 'l' && strcmp(name, "lui") != 0)
            num_cycles += 5;
        else
            num_cycles += 4;

        if(name[0] == 's' || (name[0] == 'l' && strcmp(name, "lui") != 0)) {
            num_memref++;
        }
        /* 
        printf("\n@PC=0x%08X, Opcode=0x%02X, %c Type ", pc, instruction.opcode, instruction.type);
        if(instruction.opcode == 0x00)
            printf("Function=0x%02X", instruction.function);

        printf(" (%s)", name);
        */
        pc += 4; 
        num_instructions++;
        execute(instruction);
    }
    printRegisters();
    printf("\n");
    exit(0);
  
    return 0;
}