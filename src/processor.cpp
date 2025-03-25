#include "processor.hpp"
#include <iostream>

Processor::Processor() : pc(0) {
    for (int i = 0; i < 32; i++) {
        registers[i] = 0;
    }
    memory.resize(1024, 0);
}

void Processor::loadProgram(const std::vector<uint32_t>& program) {
    memory = program;
}

void Processor::PC_calculation() {
    if (entry.PCsrc == 1) {  
        pc = entry.PC2;  
        if_id.instr = Instruction();  
    }
}

void Processor::fetch() {
    if_id.pc = pc;
    if_id.instr = Instruction(memory[pc / 4]);
    std::cout << "[IF] Fetching instruction at PC: " << pc << std::endl;
    pc += 4;
    entry.PC1 = pc;
}

void Processor::decode() {
    std::cout << "[ID] Decoding instruction at PC: " << if_id.pc << std::endl;
    id_ex.pc = if_id.pc;
    id_ex.opcode = if_id.instr.opcode;
    id_ex.rd = if_id.instr.rd;
    id_ex.rs1 = if_id.instr.rs1;
    id_ex.rs2 = if_id.instr.rs2;
    std::cout << "rs1: " << id_ex.rs1 << " rs2: " << id_ex.rs2 << std::endl;
    id_ex.regVal1 = registers[if_id.instr.rs1];
    id_ex.regVal2 = registers[if_id.instr.rs2];
    std::cout << "regVal1: " << id_ex.regVal1 << " regVal2: " << id_ex.regVal2 << std::endl;
    id_ex.imm = if_id.instr.imm;
    id_ex.funct3 = if_id.instr.funct3;
    id_ex.funct7 = if_id.instr.funct7;

    std::cout << "[ID] Decoding instruction: " << id_ex.opcode << " rs1: " << id_ex.regVal1 
              << " rs2: " << id_ex.regVal2 << " rd: " << id_ex.rd << " imm: " << id_ex.imm << std::endl;

    if (id_ex.opcode == "ADD" || id_ex.opcode == "SUB") {
        id_ex.ALUOp = 0b10;
    } else if ( id_ex.opcode == "LW" || id_ex.opcode == "SW" ) {
        id_ex.ALUOp = 0b00;
    } else if (id_ex.opcode == "ADDI") {
        id_ex.ALUOp = 0b11;
    } else if ( id_ex.opcode == "BEQ" || id_ex.opcode == "BNE" ) {
        id_ex.ALUOp = 0b01;
    }
    id_ex.ALUSrc = (id_ex.opcode == "ADDI" || id_ex.opcode == "LW" || id_ex.opcode == "SW");

    id_ex.Branch = (id_ex.opcode == "BEQ" || id_ex.opcode == "BNE" || id_ex.opcode == "BLT" || id_ex.opcode == "BGE");
    id_ex.MemRead = (id_ex.opcode == "LW");
    id_ex.MemWrite = (id_ex.opcode == "SW");
    id_ex.RegWrite = (id_ex.opcode == "ADD" || id_ex.opcode == "SUB" || id_ex.opcode == "ADDI" || id_ex.opcode == "LW");
    id_ex.MemToReg = (id_ex.opcode == "LW");

    bool branchTaken = false;
    if (id_ex.Branch) {
        if (id_ex.opcode == "BEQ") {
            branchTaken = (id_ex.regVal1 == id_ex.regVal2);
        } else if (id_ex.opcode == "BNE") {
            branchTaken = (id_ex.regVal1 != id_ex.regVal2);
        } else if (id_ex.opcode == "BLT") {
            branchTaken = (id_ex.regVal1 < id_ex.regVal2);
        } else if (id_ex.opcode == "BGE") {
            branchTaken = (id_ex.regVal1 >= id_ex.regVal2);
        }
    }

    if (branchTaken) {
        entry.PCsrc = 1;
        entry.PC2 = id_ex.pc + (id_ex.imm << 1); // PC + offset
    } else {
        entry.PCsrc = 0;
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
    ex_mem.pc_imm = id_ex.pc + (id_ex.imm << 1);
    ex_mem.rd = id_ex.rd;

    int64_t operand2 = id_ex.ALUSrc ? id_ex.imm : id_ex.regVal2;

    ex_mem.aluResult = performALUOperation(id_ex.ALUOp, id_ex.regVal1, operand2, id_ex.funct3, id_ex.funct7);

    ex_mem.zero = (id_ex.ALUOp == 0b01) && (ex_mem.aluResult == 1);

    ex_mem.regVal2 = id_ex.regVal2;

    ex_mem.Branch = id_ex.Branch;
    ex_mem.MemRead = id_ex.MemRead;
    ex_mem.MemWrite = id_ex.MemWrite;
    ex_mem.RegWrite = id_ex.RegWrite;
    ex_mem.MemToReg = id_ex.MemToReg;

    std::cout << "[EX] Executed " << id_ex.opcode << ", ALU Result: " << ex_mem.aluResult << std::endl;
}

void Processor::memoryAccess() {
    if (ex_mem.MemRead) {
        mem_wb.memData = memory[ex_mem.aluResult / 4];
        std::cout << "[MEM] Loaded data from memory: " << mem_wb.memData << std::endl;
    } else if (ex_mem.MemWrite) {
        memory[ex_mem.aluResult / 4] = ex_mem.regVal2;
        std::cout << "[MEM] Stored data in memory: " << ex_mem.regVal2 << std::endl;
    }
    else {
        std::cout << "[MEM] No memory operation" << std::endl;
    }

    mem_wb.aluResult = ex_mem.aluResult;
    mem_wb.rd = ex_mem.rd;
    mem_wb.RegWrite = ex_mem.RegWrite;
    mem_wb.MemToReg = ex_mem.MemToReg;
    entry.PCsrc = ex_mem.Branch && ex_mem.zero;
    entry.PC2 = ex_mem.pc_imm;
}

void Processor::writeBack() {
    if (mem_wb.RegWrite) {
        if (mem_wb.MemToReg) {
            registers[mem_wb.rd] = mem_wb.memData;
        } else {
            registers[mem_wb.rd] = mem_wb.aluResult;
        }
        std::cout << "[WB] Register x" << mem_wb.rd << " updated to " << registers[mem_wb.rd] << std::endl;
    }
}

void Processor::run() {
    while (pc < memory.size() * 4) {
        std::cout << "--------------------" << std::endl;        
        fetch();
        decode();
        PC_calculation();
        execute();
        memoryAccess();
        writeBack();
    }
}
