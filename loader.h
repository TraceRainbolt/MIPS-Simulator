#ifndef LOADER_H
#define LOADER_H

#define MAX_MEM_ADDR 1024

/* MIPS OPs */
#define J     0x02 
#define JAL   0x03 
#define BEQ   0x04 
#define BNE   0x05 
#define ADDI  0x08
#define ADDIU 0x09    
#define SLTI  0x0A 
#define SLTIU 0x0B 
#define ANDI  0x0C 
#define ORI   0x0D 
#define XORI  0x0E 
#define LUI   0x0F
#define LB    0x20 
#define LH    0x21
#define LW    0x23 
#define LBU   0x24 
#define LHU   0x25 
#define SB    0x28 
#define SH    0x29
#define SW    0x2B 

/* Function codes */
#define SLL     0x00
#define SRL     0x02  
#define SRA     0x03     
#define SLLV    0x04   
#define SRLV    0x06    
#define SRAV    0x07
#define JR      0x08
#define JALR    0x09
#define ADD     0x20
#define ADDU    0x21
#define SUB     0x22
#define SUBU    0x23
#define AND     0x24
#define OR      0x25
#define XOR     0x26
#define NOR     0x27
#define SLT     0x2A
#define SLTU    0x2B
#define SYSCALL 0x0C

typedef unsigned int MIPS, *MIPS_PTR;

typedef struct Instruction {
    unsigned opcode;
    char type;
    unsigned function;

    int rs;
    int rt;
    int rd;

    int imm;
    int signext;

    int shamt;
    unsigned jump_addr;

} Instruction;

int loadProgram(char *filename, MIPS mem[]);
void fetchAndDecode(Instruction *instruction, MIPS raw_hex);

void getOpName(MIPS instruction, char name[8]);

#endif

