#include "forwarding.hpp"
#include "processor.hpp"
#include <iostream>

ForwardingProcessor::ForwardingProcessor() : Processor() {
    forwardedVal1 = 0;
    forwardedVal2 = 0;
}

void ForwardingProcessor::execute() {
    // Check for forwarding conditions
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
    ex_mem.pc_imm = id_ex.pc + (id_ex.imm << 1);
    ex_mem.rd = id_ex.rd;
    ex_mem.regVal2 = forwardedVal2;

    int64_t operand2 = id_ex.ALUSrc ? id_ex.imm : ex_mem.regVal2;
    ex_mem.aluResult = performALUOperation(id_ex.ALUOp, forwardedVal1, operand2, id_ex.funct3, id_ex.funct7);

    ex_mem.zero = (id_ex.ALUOp == 0b01) && (ex_mem.aluResult == 1);
    ex_mem.Branch = id_ex.Branch;
    ex_mem.MemRead = id_ex.MemRead;
    ex_mem.MemWrite = id_ex.MemWrite;
    ex_mem.RegWrite = id_ex.RegWrite;
    ex_mem.MemToReg = id_ex.MemToReg;

    std::cout << "[EX] Executed_f " << id_ex.opcode << ", ALU Result: " << ex_mem.aluResult << std::endl;
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
