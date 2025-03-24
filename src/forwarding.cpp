#include "forwarding.hpp"
#include "processor.hpp"
#include <iostream>

ForwardingProcessor::ForwardingProcessor() : Processor() {
    forwardedVal1 = 0;
    forwardedVal2 = 0;
}

void ForwardingProcessor::execute() {
    // Check for forwarding conditions
    std::cout << "Inside execute_forward" << std::endl;
    if (checkForwardingEX_MEM_to_EX()) {
        forwardedVal1 = ex_mem.aluResult;
    } else if (checkForwardingMEM_WB_to_EX()) {
        forwardedVal1 = mem_wb.aluResult;
    } else {
        forwardedVal1 = id_ex.regVal1;
    }

    if (checkForwardingEX_MEM_to_EX()) {
        forwardedVal2 = ex_mem.aluResult;
    } else if (checkForwardingMEM_WB_to_EX()) {
        forwardedVal2 = mem_wb.aluResult;
    } else {
        forwardedVal2 = id_ex.regVal2;
    }

    // Perform ALU operation with forwarded values
    ex_mem.pc_imm = id_ex.pc + id_ex.imm;
    ex_mem.rd = id_ex.rd;
    ex_mem.regVal2 = forwardedVal2;

    if (id_ex.ALUOp == 2) { // R-type
        if (id_ex.opcode == "ADD") {
            ex_mem.aluResult = forwardedVal1 + forwardedVal2;
            std::cout << "[EX] Executed ADD, ALU Result: " << ex_mem.aluResult << std::endl;
        } else if (id_ex.opcode == "SUB") {
            ex_mem.aluResult = forwardedVal1 - forwardedVal2;
            std::cout << "[EX] Executed SUB, ALU Result: " << ex_mem.aluResult << std::endl;
        }
    } else if (id_ex.ALUOp == 1) { // I-type (ADDI)
        ex_mem.aluResult = forwardedVal1 + id_ex.imm;
        std::cout << "[EX] Executed ADDI, ALU Result: " << ex_mem.aluResult << std::endl;
    }

    ex_mem.zero = (ex_mem.aluResult == 0);
    ex_mem.Branch = id_ex.Branch;
    ex_mem.MemRead = id_ex.MemRead;
    ex_mem.MemWrite = id_ex.MemWrite;
    ex_mem.RegWrite = id_ex.RegWrite;
    ex_mem.MemToReg = id_ex.MemToReg;
}

void ForwardingProcessor::memoryAccess() {
    // Forward memory read/write results
    if (ex_mem.MemRead) {
        mem_wb.memData = memory[ex_mem.aluResult / 4];
    } else if (ex_mem.MemWrite) {
        memory[ex_mem.aluResult / 4] = ex_mem.regVal2;
    }

    mem_wb.aluResult = ex_mem.aluResult;
    mem_wb.rd = ex_mem.rd;
    mem_wb.RegWrite = ex_mem.RegWrite;
    mem_wb.MemToReg = ex_mem.MemToReg;
}

bool ForwardingProcessor::checkForwardingEX_MEM_to_ID_EX() {
    return ex_mem.RegWrite && ex_mem.rd != 0 &&
           (ex_mem.rd == id_ex.rs1 || ex_mem.rd == id_ex.rs2);
}

bool ForwardingProcessor::checkForwardingMEM_WB_to_ID_EX() {
    return mem_wb.RegWrite && mem_wb.rd != 0 &&
           (mem_wb.rd == id_ex.rs1 || mem_wb.rd == id_ex.rs2);
}

bool ForwardingProcessor::checkForwardingEX_MEM_to_EX() {
    return ex_mem.RegWrite && ex_mem.rd != 0 &&
           (ex_mem.rd == id_ex.rs1 || ex_mem.rd == id_ex.rs2);
}

bool ForwardingProcessor::checkForwardingMEM_WB_to_EX() {
    return mem_wb.RegWrite && mem_wb.rd != 0 &&
           (mem_wb.rd == id_ex.rs1 || mem_wb.rd == id_ex.rs2);
}
