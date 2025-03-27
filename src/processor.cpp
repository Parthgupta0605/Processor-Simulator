#include "processor.hpp"
#include <iostream>

Processor::Processor() : pc(0) {
    for (int i = 0; i < 32; i++) {
        registers[i] = 0;
    }
    // Resize memory to 1024 elements, each initialized to a tuple (0, Instruction())
    
}

void Processor::loadProgram(const std::vector<uint32_t>& program) {
    // Load the program into memory, updating only the first element of the tuple
    memory.resize(program.size(), std::make_tuple(0, Instruction()));

    for (size_t i = 0; i < program.size(); i++) {
        if (i < memory.size()) {
            memory[i] = std::make_tuple(program[i], Instruction(program[i]));
        }
    }
    current = 0;
    
}


void Processor::fetch() {
    if_id.pc = pc;
    if_id.instr = std::get<1>(memory[pc / 4]);
    std::get<1>(memory[pc / 4]).stage = 2;
    std::cout << "[IF] Fetching instruction at PC: " << pc << std::endl;
    pc += 4;
    
    
}

void Processor::decode() {
    if (NOP) {NOP = false;if_id.pc=ex_mem.pc_imm;}
    else {
    std::get<1>(memory[if_id.pc / 4]).stage = 3;
    id_ex.instr = if_id.instr;
    std::cout << "[ID] Decoding instruction at PC: " << if_id.pc << std::endl;
    id_ex.pc = if_id.pc;
    id_ex.opcode = if_id.instr.opcode;
    id_ex.rd = if_id.instr.rd;
    id_ex.rs1 = if_id.instr.rs1;
    id_ex.rs2 = if_id.instr.rs2;
    id_ex.regVal1 = registers[if_id.instr.rs1];
    id_ex.regVal2 = registers[if_id.instr.rs2];
    id_ex.imm = if_id.instr.imm;
    id_ex.funct3 = if_id.instr.funct3;
    id_ex.funct7 = if_id.instr.funct7;

    if (id_ex.opcode == "ADD" || id_ex.opcode == "SUB") {
        id_ex.ALUOp = 0b10;
    } else if ( id_ex.opcode == "LW" || id_ex.opcode == "SW" || id_ex.opcode == "LB" || id_ex.opcode == "SB") {
        id_ex.ALUOp = 0b00;
    } else if (id_ex.opcode == "ADDI") {
        id_ex.ALUOp = 0b11;
    } else if ( id_ex.opcode == "BEQ" || id_ex.opcode == "BNE" ) {
        id_ex.ALUOp = 0b01;
    }
    id_ex.ALUSrc = (id_ex.opcode == "ADDI" || id_ex.opcode == "LW" || id_ex.opcode == "SW" || id_ex.opcode == "LB" || id_ex.opcode == "SB");

    id_ex.Branch = (id_ex.opcode == "BEQ" || id_ex.opcode == "BNE" || id_ex.opcode == "BLT" || id_ex.opcode == "BGE");
    id_ex.MemRead = (id_ex.opcode == "LW" || id_ex.opcode == "LB");
    id_ex.MemWrite = (id_ex.opcode == "SW" || id_ex.opcode == "SB");
    id_ex.RegWrite = (id_ex.opcode == "ADD" || id_ex.opcode == "SUB" || id_ex.opcode == "ADDI" || id_ex.opcode == "LW" || id_ex.opcode == "LB");
    id_ex.MemToReg = (id_ex.opcode == "LW" || id_ex.opcode == "LB");
    if (id_ex.opcode == "JAL") {
        id_ex.ALUOp = 0b11;
        id_ex.RegWrite = true;
        id_ex.returnAddress = id_ex.pc + 4; 
    }
    
}



}

int64_t Processor::performALUOperation(int ALUOp, int64_t operand1, int64_t operand2, int funct3, int funct7) {
    switch (ALUOp) {
        case 0b00:  // Load/Store (address calculation)
            return operand1 + operand2;

        case 0b01:  // Branch comparison
            if (funct3 == 0b000) return (operand1 == operand2) ? 1: 0; // BEQ
            if (funct3 == 0b001) return (operand1 != operand2) ? 1: 0; // BNE
            if (funct3 == 0b100) return (operand1 < operand2) ? 1 : 0; // BLT
            if (funct3 == 0b101) return (operand1 >= operand2) ? 1 : 0; // BGE
            if (funct3 == 0b110) return ((uint64_t)operand1 < (uint64_t)operand2) ? 1 : 0; // BLTU
            if (funct3 == 0b111) return ((uint64_t)operand1 >= (uint64_t)operand2) ? 1 : 0; // BGEU
            break;

        case 0b10:  // R-type ALU operations
            if (funct3 == 0b000) {
                return (funct7 == 0b0100000) ? operand1 - operand2 : operand1 + operand2; // SUB if funct7=0x20, else ADD
            }
            if (funct3 == 0b111) return operand1 & operand2;  // AND
            if (funct3 == 0b110) return operand1 | operand2;  // OR
            if (funct3 == 0b100) return operand1 ^ operand2;  // XOR
            if (funct3 == 0b001) return operand1 << operand2;  // SLL
            if (funct3 == 0b101) {
                return (funct7 == 0b0100000) ? operand1 >> operand2 : (uint64_t)operand1 >> operand2; // SRA if funct7=0x20, else SRL
            }
            if (funct3 == 0b010) return (operand1 < operand2) ? 1 : 0;  // SLT
            if (funct3 == 0b011) return ((uint64_t)operand1 < (uint64_t)operand2) ? 1 : 0;  // SLTU
            break;

        case 0b11:  // Special cases (Jumps, LUI, AUIPC)
            if (funct3 == 0b000) {  // JALR
                return (operand1 + operand2) & ~1;
            }
            if (funct3 == -1) {  // LUI, AUIPC have no funct3
                if (funct7 == 0b0000000) return operand2 << 12; // LUI
                if (funct7 == 0b0000001) return operand1 + (operand2 << 12); // AUIPC
            }
            break;

        default:
            std::cerr << "Unknown ALUOp: " << ALUOp << std::endl;
            return 0;
    }
    return 0;
}

void Processor::execute() {
    if (NOP) NOP = false;
    else {
    std::get<1>(memory[id_ex.pc / 4]).stage = 4;
    ex_mem.instr = id_ex.instr;
    ex_mem.pc_imm = id_ex.pc + id_ex.imm ;
    
    ex_mem.pc = id_ex.pc;
    ex_mem.rd = id_ex.rd;
    ex_mem.rs1 = id_ex.rs1;
    ex_mem.rs2 = id_ex.rs2;

    int64_t operand2 = id_ex.ALUSrc ? id_ex.imm : id_ex.regVal2;

    ex_mem.aluResult = performALUOperation(id_ex.ALUOp, id_ex.regVal1, operand2, id_ex.funct3, id_ex.funct7);

    ex_mem.zero = (id_ex.ALUOp == 0b01) && (ex_mem.aluResult == 1);

    ex_mem.regVal2 = id_ex.regVal2;

    ex_mem.Branch = id_ex.Branch;
    ex_mem.MemRead = id_ex.MemRead;
    ex_mem.MemWrite = id_ex.MemWrite;
    ex_mem.RegWrite = id_ex.RegWrite;
    ex_mem.MemToReg = id_ex.MemToReg;

    if (id_ex.opcode == "JAL") {
        ex_mem.returnAddress = id_ex.returnAddress;  // Pass return address
        pc = ex_mem.pc_imm;  // Update PC to jump target
        NOP=true;
        std::cout << "[EX] Executed JAL, Jump to: " << ex_mem.pc_imm 
                  << ", Return Address: " << ex_mem.returnAddress << std::endl;
    }
    
    else 
    {
    std::cout << "[EX] Executed " << id_ex.opcode << ", at PC: " << ex_mem.pc << std::endl;
    }
}


}

void Processor::memoryAccess() {
    if (NOP) NOP = false;
    else {
    
    mem_wb.instr = ex_mem.instr;
    std::get<1>(memory[ex_mem.pc / 4]).stage = 5;
    if (ex_mem.RegWrite && std::get<1>(memory[current]).rd != 0 && (std::get<1>(memory[current]).rd == std::get<1>(memory[current+1]).rs1 || std::get<1>(memory[current]).rd == std::get<1>(memory[current+1]).rs2)){
        
        ex_stall = true;
        id_stall = true;
        if_stall = true;
    }
    if (ex_mem.MemWrite && std::get<1>(memory[current]).rd != 0 && (std::get<1>(memory[current]).rd == std::get<1>(memory[current+1]).rs1 || std::get<1>(memory[current]).rd == std::get<1>(memory[current+1]).rs2)){
        
        ex_stall = true;
        id_stall = true;
        if_stall = true;
        
    }

    if (ex_mem.MemRead) {
        mem_wb.memData = std::get<0>(memory[ex_mem.aluResult / 4]);
        std::cout << "[MEM] Loaded data from memory: " << mem_wb.memData << ", at PC: " <<mem_wb.pc<< std::endl;
    } else if (ex_mem.MemWrite) {
        std::get<0>(memory[ex_mem.aluResult / 4]) = ex_mem.regVal2;
        std::cout << "[MEM] Stored data in memory: " << ex_mem.regVal2 << ", at PC: " <<mem_wb.pc << std::endl;
    }
    else {
        std::cout << "[MEM] No memory operation, at PC: " <<mem_wb.pc << std::endl;
    }

    mem_wb.aluResult = ex_mem.aluResult;
    mem_wb.rd = ex_mem.rd;
    mem_wb.MemWrite = ex_mem.MemWrite;
    mem_wb.MemRead = ex_mem.MemRead;
    mem_wb.RegWrite = ex_mem.RegWrite;
    mem_wb.MemToReg = ex_mem.MemToReg;
    mem_wb.pc = ex_mem.pc;
    mem_wb.pc_imm = ex_mem.pc_imm;
    mem_wb.instr = ex_mem.instr;
    if (ex_mem.instr.opcode == "JAL") {
        mem_wb.returnAddress = ex_mem.returnAddress; // Pass the return address
        NOP = true;
    }
}


}

void Processor::writeBack() {
    if (NOP) NOP = false;
    else {
    if (mem_wb.RegWrite) {
        if (mem_wb.MemToReg) {
            registers[mem_wb.rd] = mem_wb.memData;
        } else {
            registers[mem_wb.rd] = mem_wb.aluResult;
        }
        std::cout << "[WB] Register x" << mem_wb.rd << " updated to " << registers[mem_wb.rd] << ", at PC: " <<current*4 << std::endl;
    }
    else {
        std::cout << "[WB] No writeback operation, at PC: " <<current*4<< std::endl;
    }
    
    if (mem_wb.instr.opcode == "JAL" && mem_wb.RegWrite) {
        registers[mem_wb.rd] = mem_wb.returnAddress;
        NOP = true;
        current = mem_wb.pc_imm / 4;
        std::cout << "[WB] JAL: Register x" << mem_wb.rd 
                  << " updated with return address: " << mem_wb.returnAddress << ", at PC: " <<current*4<< std::endl;
    }
    else {
    if (mem_wb.RegWrite && std::get<1>(memory[current]).rd != 0 && (std::get<1>(memory[current]).rd == std::get<1>(memory[current+1]).rs1 || std::get<1>(memory[current]).rd == std::get<1>(memory[current+1]).rs2)){
        mem_stall = true;
        ex_stall = true;
        id_stall = true;
        if_stall = true;   
    }
    else if (mem_wb.RegWrite && std::get<1>(memory[current]).rd != 0 && (std::get<1>(memory[current]).rd == std::get<1>(memory[current+2]).rs1 || std::get<1>(memory[current]).rd == std::get<1>(memory[current+2]).rs2)){
        switch (std::get<1>(memory[current]).stage) {
            case 1:
            mem_stall = true;
            ex_stall = true;
            id_stall = true;
            if_stall = false;
            break;
            case 2:
            mem_stall = true;
            ex_stall = true;
            id_stall = false;
            if_stall = false;
            break;
            case 3:
            mem_stall = true;
            ex_stall = false;
            id_stall = false;
            if_stall = false;
            break;
            case 4:
            mem_stall = false;
            ex_stall = false;
            id_stall = false;
            if_stall = false;
            break;
            default:
                break;
        }
    }
    else if (mem_wb.MemWrite && std::get<1>(memory[current]).rd != 0 && (std::get<1>(memory[current]).rd == std::get<1>(memory[current+1]).rs1 || std::get<1>(memory[current]).rd == std::get<1>(memory[current+1]).rs2)){
        switch (std::get<1>(memory[current+1]).stage) {
            case 1:
            mem_stall = true;
            ex_stall = true;
            id_stall = true;
            if_stall = false;
            break;
            case 2:
            mem_stall = true;
            ex_stall = true;
            id_stall = false;
            if_stall = false;
            break;
            case 3:
            mem_stall = true;
            ex_stall = false;
            id_stall = false;
            if_stall = false;
            break;
            case 4:
            mem_stall = false;
            ex_stall = false;
            id_stall = false;
            if_stall = false;
            break;
            default:
                break;
        }
    }
        else {
        switch (std::get<1>(memory[current+1]).stage) {
            case 1:
            mem_stall = true;
            ex_stall = true;
            id_stall = true;
            if_stall = false;
            break;
            case 2:
            mem_stall = true;
            ex_stall = true;
            id_stall = false;
            if_stall = false;
            break;
            case 3:
            mem_stall = true;
            ex_stall = false;
            id_stall = false;
            if_stall = false;
            break;
            case 4:
            mem_stall = false;
            ex_stall = false;
            id_stall = false;
            if_stall = false;
            break;
            default:
                break;
        }
        
    }
    
    
    current++;
}


}
    
}


