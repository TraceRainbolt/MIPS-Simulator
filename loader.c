#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "loader.h"

#include "mips_asm_header.h"

MB_HDR mb_hdr;      /* Header area */

char getType(unsigned opcode){
    if(opcode == 0x00)
        return 'R';
    else if(opcode == 2 || opcode == 3)
        return 'J';
    return 'I';
}

void getFunctionName(unsigned instruction, char name[8]){
    unsigned function = instruction & 0x3F;
    switch(function) {
        case 0x0:
            strcpy(name, "sll");
            break;
        case 0x2:
            strcpy(name, "srl");
            break;
        case 0x3:
            strcpy(name, "sra");
            break;
        case 0x4:
            strcpy(name, "sllv");
            break;
        case 0x6:
            strcpy(name, "srlv");
            break;
        case 0x7:
            strcpy(name, "srav");
            break;
        case 0x8:
            strcpy(name, "jr");
            break;
        case 0x9:
            strcpy(name, "jalr");
            break;
        case 0x20:
            strcpy(name, "add");
            break;
        case 0x21:
            strcpy(name, "addu");
            break;
        case 0x22:
            strcpy(name, "sub");
            break;
        case 0x23:
            strcpy(name, "subu");
            break;
        case 0x24:
            strcpy(name, "and");
            break;
        case 0x25:
            strcpy(name, "or");
            break;
        case 0x26:
            strcpy(name, "xor");
            break;
        case 0x27:
            strcpy(name, "nor");
            break;
        case 0x2A:
            strcpy(name, "slt");
            break;
        case 0x2B:
            strcpy(name, "sltu");
            break;
        case 0x0C:
            strcpy(name, "syscall");
            break;
        default:
            strcpy(name, "nop");
    }
}

void getOpName(MIPS raw_hex, char name[8]){
    unsigned opcode = raw_hex >> 26;
    switch(opcode) {
        case 0x00:
            getFunctionName(raw_hex, name);
            break;
        case 0x02:
            strcpy(name, "j");
            break;
        case 0x03:
            strcpy(name, "jal");
            break;
        case 0x04:
            strcpy(name, "beq");
            break;
        case 0x05:
            strcpy(name, "bne");
            break;
        case 0x08:
            strcpy(name, "addi");
            break;
        case 0x09:
            strcpy(name, "addu");
            break;       
        case 0x0A:
            strcpy(name, "slti");
            break;
        case 0x0B:
            strcpy(name, "sltiu");
            break;
        case 0x0C:
            strcpy(name, "andi");
            break;
        case 0x0D:
            strcpy(name, "ori");
            break;
        case 0x0E:
            strcpy(name, "xori");
            break;
        case 0x0F:
            strcpy(name, "lui");
            break;
        case 0x20:
            strcpy(name, "lb");
            break;
        case 0x21:
            strcpy(name, "lh");
            break;
        case 0x23:
            strcpy(name, "lw");
            break;
        case 0x24:
            strcpy(name, "lbu");
            break;
        case 0x25:
            strcpy(name, "lhu");
            break;
        case 0x28:
            strcpy(name, "sb");
            break;
        case 0x29:
            strcpy(name, "sh");
            break;
        case 0x2B:
            strcpy(name, "sw");
            break;
        default:
            strcpy(name, "nop");
    }
}

void getRegString(int num, char reg[8]){
    reg[0] = 0;
    switch(num){
        case 0:
            strcpy(reg, "$zero");
            break;
        case 1:
            strcpy(reg, "$at");
            break;
        case 28:
            strcpy(reg, "$gp");
            break;
        case 29:
            strcpy(reg, "$sp");
            break;
        case 30:
            strcpy(reg, "$fp");
            break;
        case 31:
            strcpy(reg, "$ra");
            break;
    }
    if(reg[0] != 0)
        return;
    if(num <= 3){
        strcpy(reg, "$v");
        reg[2] = 0x30 + num - 2;
        reg[3] = '\0';
    }
    else if(num <= 7){
        strcpy(reg, "$a");
        reg[2] = 0x30 + num - 4;
        reg[3] = '\0';     
    }
    else if(num <= 15){
        strcpy(reg, "$t");
        reg[2] = 0x30 + num - 8;
        reg[3] = '\0';     
    }
    else if(num <= 23){
        strcpy(reg, "$s");
        reg[2] = 0x30 + num - 16;
        reg[3] = '\0';     
    }
    else if(num <= 25){
        strcpy(reg, "$t");
        reg[2] = 0x30 + num - 24;
        reg[3] = '\0';     
    }
    else if(num <= 27){
        strcpy(reg, "$t");
        reg[2] = 0x30 + num - 26;
        reg[3] = '\0';     
    }
}

int signExtend(int instruction) {
    int value = (0x0000FFFF & instruction);
    int mask = 0x00008000;
    if (mask & instruction) {
        value += 0xFFFF0000;
    }
    return value;
}


/* Put all the instruction components into a nice accessible struct */
void fetchAndDecode(Instruction *instruction, MIPS raw_hex){
    unsigned opcode = raw_hex >> 26;
    int imm = raw_hex & 0x7FFF;

    instruction->opcode = opcode;
    instruction->type = getType(opcode);
    instruction->function = raw_hex & 0x3F;

    instruction->rs = (raw_hex >> 21) & 0x1F;
    instruction->rt = (raw_hex >> 16) & 0x1F;
    instruction->rd = (raw_hex >> 11) & 0x1F;

    instruction->imm = imm;
    instruction->signext = signExtend(imm);

    instruction->shamt = (raw_hex >> 6) & 0x1F;
    instruction->jump_addr = raw_hex & 0x3FFFFFF;
}

int loadProgram(char *filename, MIPS mem[]){
    FILE *fd;
    int n;
    int memp;

/* format the MIPS Binary header */

    fd = fopen(filename, "rb");
    if(fd == NULL) { printf("\nCouldn't load test case - quitting.\n"); exit(99); }
  
    memp = 0;     /* This is the memory pointer, a byte offset */

/* read the header and verify it. */
    fread((void *) &mb_hdr, sizeof(mb_hdr), 1, fd);
  
    if(strcmp(mb_hdr.signature, "~MB")){
        printf("\nThis isn't really a mips_asm binary file - quitting.\n"); 
        exit(98);
    }
    
    printf("\n%s Loaded ok, program size=%d bytes.\n\n", filename, mb_hdr.size);
  
/*   read the binary code a word at a time */
    do {
        n = fread((void *) &mem[memp/4], 4, 1, fd); /* note div/4 to make word index */
        if (n) 
            memp += 4;    /* Increment byte pointer by size of instr */
        else
            break;       
    } while (memp < MAX_MEM_ADDR);
  
    fclose(fd);

    return memp;
}
