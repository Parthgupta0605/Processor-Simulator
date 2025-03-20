#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP

#include <string>
#include <cstdint>
struct Instruction {
    uint32_t M_Code;   //machine code (32 bits)
    std::string opcode;   //string for storing opcode
    int rd;  //destination register
    int rs1, rs2;   //source registers
    int imm;  //immediate value 
    int funct3, funct7; //function codes         
    int cycleEntered[5]; //For tracking the cycle entered in each stage

    Instruction(uint32_t code); 
    void decode();               
};
#endif
