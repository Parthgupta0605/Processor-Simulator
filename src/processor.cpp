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

void Processor::fetch() {
    if_id.pc = pc;
    if_id.instr = Instruction(memory[pc / 4]);
    std::cout << "[IF] Fetching instruction at PC: " << pc << std::endl;
    pc += 4;
}

void Processor::decode() {
    std::cout << "[ID] Decoding instruction at PC: " << if_id.pc << std::endl;
    id_ex.pc = if_id.pc;
    id_ex.opcode = if_id.instr.opcode;
    id_ex.rd = if_id.instr.rd;
    id_ex.regVal1 = registers[if_id.instr.rs1];
    id_ex.regVal2 = registers[if_id.instr.rs2];
    id_ex.imm = if_id.instr.imm;
    id_ex.funct3 = if_id.instr.funct3;
    id_ex.funct7 = if_id.instr.funct7;

    std::cout << "[ID] Decoding instruction: " << id_ex.opcode << " rs1: " << id_ex.regVal1 
              << " rs2: " << id_ex.regVal2 << " rd: " << id_ex.rd << " imm: " << id_ex.imm << std::endl;

    id_ex.ALUSrc = (id_ex.opcode == "ADDI" || id_ex.opcode == "LW");
    id_ex.Branch = (id_ex.opcode == "BEQ" || id_ex.opcode == "BNE");
    id_ex.MemRead = (id_ex.opcode == "LW");
    id_ex.MemWrite = (id_ex.opcode == "SW");
    id_ex.RegWrite = (id_ex.opcode == "ADD" || id_ex.opcode == "SUB" || id_ex.opcode == "ADDI" || id_ex.opcode == "LW");
    id_ex.MemToReg = (id_ex.opcode == "LW");
}

void Processor::execute() {
    ex_mem.pc_imm = id_ex.pc + id_ex.imm;
    ex_mem.rd = id_ex.rd;

    if (id_ex.opcode == "ADD") {
        ex_mem.aluResult = id_ex.regVal1 + id_ex.regVal2;
    } else if (id_ex.opcode == "SUB") {
        ex_mem.aluResult = id_ex.regVal1 - id_ex.regVal2;
    } else if (id_ex.opcode == "ADDI") {
        ex_mem.aluResult = id_ex.regVal1 + id_ex.imm;
    }

    ex_mem.zero = (id_ex.regVal1 == id_ex.regVal2);

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

    mem_wb.aluResult = ex_mem.aluResult;
    mem_wb.rd = ex_mem.rd;
    mem_wb.RegWrite = ex_mem.RegWrite;
    mem_wb.MemToReg = ex_mem.MemToReg;
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
        execute();
        memoryAccess();
        writeBack();
    }
}
