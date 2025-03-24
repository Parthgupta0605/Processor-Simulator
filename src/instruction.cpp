#include "instruction.hpp"
#include <iostream>

Instruction::Instruction(uint32_t code) : M_Code(code) {
    decode();
}

void Instruction::decode() {
    Instruction instr;
    uint32_t opcode_mask = 0x7F;  // Last 7 bits (bits 0-6)
    uint32_t rd_mask = 0xF80;     // Bits 7-11
    uint32_t funct3_mask = 0x7000; // Bits 12-14
    uint32_t rs1_mask = 0xF8000;  // Bits 15-19
    uint32_t rs2_mask = 0x1F00000; // Bits 20-24
    uint32_t funct7_mask = 0xFE000000; // Bits 25-31

    uint32_t opcode_num = M_Code & opcode_mask;
    switch (opcode_num) {
        case 0x33: // R-Type
           rd = (M_Code & rd_mask) >> 7;
           funct3 = (M_Code & funct3_mask) >> 12;
           rs1 = (M_Code & rs1_mask) >> 15;
           rs2 = (M_Code & rs2_mask) >> 20;
           funct7 = (M_Code & funct7_mask) >> 25;
            if (funct3 == 0x0 &&funct7 == 0x00) opcode = "ADD";
            else if (funct3 == 0x0 &&funct7 == 0x20) opcode = "SUB";
            else if (funct3 == 0x7) opcode = "AND";
            else if (funct3 == 0x6) opcode = "OR";
            else if (funct3 == 0x4) opcode = "XOR";
            break;
        
        case 0x13: // I-Type (ADDI, ANDI, ORI)
            rd = (M_Code & rd_mask) >> 7;
            funct3 = (M_Code & funct3_mask) >> 12;
            rs1 = (M_Code & rs1_mask) >> 15;
            imm = static_cast<int32_t>(M_Code) >> 20; 
            if (funct3 == 0x0) opcode = "ADDI";
            else if (funct3 == 0x7) opcode = "ANDI";
            else if (funct3 == 0x6) opcode = "ORI";
            break;

        case 0x03: // Load (I-Type)
            rd = (M_Code & rd_mask) >> 7;
            funct3 = (M_Code & funct3_mask) >> 12;
            rs1 = (M_Code & rs1_mask) >> 15;
            imm = static_cast<int32_t>(M_Code) >> 20;
            if (funct3 == 0x0) opcode = "LB";
            else if (funct3 == 0x2) opcode = "LW";
            break;

        case 0x23: // Store (S-Type)
            funct3 = (M_Code & funct3_mask) >> 12;
            rs1 = (M_Code & rs1_mask) >> 15;
            rs2 = (M_Code & rs2_mask) >> 20;
            imm = static_cast<int32_t>(((M_Code >> 7) & 0x1F) | ((M_Code >> 25) << 5));
            if (funct3 == 0x0) opcode = "SB";
            else if (funct3 == 0x2) opcode = "SW";
            break;

        case 0x63: // Branch (B-Type)
            funct3 = (M_Code & funct3_mask) >> 12;
            rs1 = (M_Code & rs1_mask) >> 15;
            rs2 = (M_Code & rs2_mask) >> 20;
            imm = static_cast<int32_t>(((M_Code >> 7) & 0x1E) | ((M_Code >> 20) & 0x7E0) |
                                        ((M_Code >> 25) & 0xFE0) | ((M_Code >> 31) << 12)); 
            imm = imm << 19 >> 19;
            if (funct3 == 0x0) opcode = "BEQ";
            else if (funct3 == 0x1) opcode = "BNE";
            break;

        case 0x6F: // JAL (J-Type)
            rd = (M_Code & rd_mask) >> 7;
            imm = static_cast<int32_t>(((M_Code >> 12) & 0xFF) | ((M_Code >> 20) & 0x1) << 11 |
                                        ((M_Code >> 21) & 0x3FF) << 1 | ((M_Code >> 31) << 20));
            imm = imm << 11 >> 11;
            opcode = "JAL";
            break;

        case 0x67: // JALR (I-Type)
            rd = (M_Code & rd_mask) >> 7;
            funct3 = (M_Code & funct3_mask) >> 12;
            rs1 = (M_Code & rs1_mask) >> 15;
            imm = static_cast<int32_t>(M_Code) >> 20;
            opcode = "JALR";
            break;

        default:
            opcode = "UNKNOWN";
            break;
    }
    
    std::cout << "Decoded: " << opcode << " rd: " << rd << " rs1: " << rs1 
              << " rs2: " << rs2 << " funct3: " << funct3 
              << " funct7: " << funct7 << " imm: " << imm << std::endl;
}
