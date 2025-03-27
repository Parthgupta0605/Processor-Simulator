#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP

#include <string>
#include <cstdint>
struct Instruction {
    uint32_t M_Code;   //machine code (32 bits)
    std::string opcode;   //string for storing opcode
    int rd;  //destination register
    int rs1, rs2;   //source registers
    int64_t imm;  //immediate value 
    int funct3, funct7; //function codes         
    int cycleEntered[5] = {-1, -1, -1, -1, -1}; //For tracking the cycle entered in each stage
    int stage;

    Instruction() : M_Code(0), opcode("NOP"), rd(-1), rs1(-1), rs2(-1), imm(-1), funct3(-1), funct7(-1),stage(1) {
        for (int i = 0; i < 5; i++) cycleEntered[i] = 0;
    }
    Instruction(uint32_t code); 
    void decode();               
};
#endif
