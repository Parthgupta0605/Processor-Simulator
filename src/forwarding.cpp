#include "forwarding.hpp"
#include "processor.hpp"
#include <iostream>

ForwardingProcessor::ForwardingProcessor() : Processor() {
    forwardedVal1 = 0;
    forwardedVal2 = 0;
}

void ForwardingProcessor::execute() {
    // Check for forwarding conditions
    if (NOP) NOP = false;
    else {
    // Perform ALU operation with forwarded values
    std::get<1>(memory[id_ex.pc / 4]).stage = 4;
    ex_mem.instr = id_ex.instr;
    ex_mem.pc_imm = id_ex.pc + id_ex.imm ;
    ex_mem.pc = id_ex.pc;
    ex_mem.rd = id_ex.rd;
    ex_mem.rs1 = id_ex.rs1;
    ex_mem.rs2 = id_ex.rs2;
    ex_mem.regVal2 = forwardedVal2;

    int64_t operand2 = id_ex.ALUSrc ? id_ex.imm : ex_mem.regVal2;
    ex_mem.aluResult = performALUOperation(id_ex.ALUOp, forwardedVal1, operand2, id_ex.funct3, id_ex.funct7);

    ex_mem.zero = (id_ex.ALUOp == 0b01) && (ex_mem.aluResult == 1);
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
    else{
    std::cout << "[EX] Executed_f " << id_ex.opcode << ", at PC: " << ex_mem.pc << std::endl;
    }
    }
}

void ForwardingProcessor::memoryAccess() {
    // Forward memory read/write results
    if (NOP) NOP = false;
    else {
    mem_wb.instr = ex_mem.instr;
    std::get<1>(memory[ex_mem.pc / 4]).stage = 5;

    if (ex_mem.MemRead && ex_mem.rd != 0 ){
        if (ex_mem.MemWrite && (ex_mem.rd == id_ex.rs1 || ex_mem.rd == id_ex.rs2));
        else {
        ex_stall = true;
        id_stall = true;
        if_stall = true;
        }
    }
    if (ex_mem.MemRead) {
        mem_wb.memData = std::get<0>(memory[ex_mem.aluResult / 4]);
        std::cout << "[MEM] Loaded data from memory: " << mem_wb.memData << ", at PC: " <<mem_wb.pc<< std::endl;
    } else if (ex_mem.MemWrite) {
        std::get<0>(memory[ex_mem.aluResult / 4]) = ex_mem.regVal2;
        std::cout << "[MEM] Stored data in memory: " << ex_mem.regVal2 << ", at PC: " <<mem_wb.pc<< std::endl;
    }
    else {
        std::cout << "[MEM] No memory operation, at PC: " <<mem_wb.pc << std::endl;
    }

    mem_wb.aluResult = ex_mem.aluResult;
    mem_wb.rd = ex_mem.rd;
    mem_wb.RegWrite = ex_mem.RegWrite;
    mem_wb.MemToReg = ex_mem.MemToReg;
    mem_wb.pc = ex_mem.pc;
    mem_wb.instr = ex_mem.instr;
    mem_wb.pc_imm = ex_mem.pc_imm;
    if (ex_mem.instr.opcode == "JAL") {
        mem_wb.returnAddress = ex_mem.returnAddress; // Pass the return address
        NOP = true;
    }
}
}
void ForwardingProcessor::writeBack() {
    // Write back results to register file
    if (NOP) NOP = false;
    else {
    if (mem_wb.RegWrite) {
        if (mem_wb.MemToReg) {
            registers[mem_wb.rd] = mem_wb.memData;
        } else {
            registers[mem_wb.rd] = mem_wb.aluResult;
        }
        std::cout << "[WB] Register x" << mem_wb.rd << " updated to " << registers[mem_wb.rd] << ", at PC: " <<current*4<< std::endl;
    }
    else {
        std::cout << "[WB] No writeback operation, at PC: " <<current*4<< std::endl;
    }
    if (mem_wb.instr.opcode == "JAL" && mem_wb.RegWrite) {
        registers[mem_wb.rd] = mem_wb.returnAddress;
        NOP = true;
        current = mem_wb.pc_imm / 4;
        std::cout << "[WB] JAL: Register x" << mem_wb.rd 
                  << " updated with return address: " << mem_wb.returnAddress << ", at PC: " <<current*4 << std::endl;
    }
    else{
    if (ex_mem.MemRead && std::get<1>(memory[current]).rd != 0 ){
        if (ex_mem.MemWrite && (std::get<1>(memory[current]).rd == std::get<1>(memory[current+1]).rs1 || std::get<1>(memory[current]).rd == std::get<1>(memory[current+1]).rs2));
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
    }
    current++;
}
}
}