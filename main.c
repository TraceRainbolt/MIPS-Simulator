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

/* Create memory and registers */
MIPS mem[MAX_MEM_ADDR];
MIPS registers[NUM_REGS];
MIPS pc = 0;

void initRegisters(MIPS registers[]){
    int i;
    for(i = 0; i < NUM_REGS; i++)
        registers[i] = 0;
}

void executeFunction(Instruction instruction){
    switch(instruction.opcode) {
        case SLL:
 
            break;
        case SRL:

            break;
        case SRA:

            break;
        case SLLV:

            break;
        case SRLV:

            break;
        case SRAV:

            break;
        case JR:

            break;
        case JALR:

            break;
        case ADD:

            break;
        case ADDU:

            break;
        case SUB:

            break;
        case SUBU:

            break;
        case AND:

            break;
        case OR:

            break;
        case XOR:

            break;
        case NOR:

            break;
        case SLT:

            break;
        case SLTU:

            break;
        case SYSCALL:

            break;
    }
}

void execute(Instruction instruction){
    switch(instruction.opcode) {
        /* Gotta function code to handle */
        case 0x00:
            executeFunction(instruction);
            break;
        case J:
            /* do J */
            break;
        case JAL:

            break;
        case BEQ:

            break;
        case BNE:

            break;
        case ADDI:

            break;      
        case SLTI:

            break;
        case SLTIU:

            break;
        case ANDI:

            break;
        case ORI:

            break;
        case XORI:

            break;
        case LUI:

            break;
        case LB:

            break;
        case LH:

            break;
        case LW:

            break;
        case LBU:

            break;
        case LHU:

            break;
        case SB:

            break;
        case SH:

            break;
        case SW:

            break;
    }
}

int main(){
    /* File to be ran */
    char filename[] = "testcase2.mb";
    initRegisters(registers);

    int memp = loadProgram(filename, mem);
    while(pc < memp){
        int raw_hex = mem[pc / 4];
        Instruction instruction;
        char name[8];

        fetchAndDecode(&instruction, raw_hex);

        execute(instruction);

        /**** Debug print ****** 
        
        getOpName(raw_hex, name);

        printf("\n@PC=0x%08X, Opcode=0x%02X, %c Type ", pc, instruction.opcode, instruction.type);
        if(instruction.opcode == 0x00)
            printf("Function=0x%02X", instruction.function);

        printf(" (%s)", name); 

        pc += 4; */
    }
  
    printf("\n");
    exit(0);
  
    return 0;
}