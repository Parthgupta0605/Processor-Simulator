#include "pipeline.hpp"
#include "processor.hpp"
#include <iostream>

void initializePipeline(Processor &processor) {
    processor.pc = 0;

    // Initialize pipeline registers
    processor.if_id.pc = 0;
    processor.if_id.instr = Instruction(0);

    processor.id_ex.pc = 0;
    processor.id_ex.opcode = "";
    processor.id_ex.rd = 0;
    processor.id_ex.imm = 0;
    processor.id_ex.funct3 = 0;
    processor.id_ex.funct7 = 0;
    processor.id_ex.regVal1 = 0;
    processor.id_ex.regVal2 = 0;
    processor.id_ex.ALUSrc = false;
    processor.id_ex.ALUOp = 0;
    processor.id_ex.Branch = false;
    processor.id_ex.MemRead = false;
    processor.id_ex.MemWrite = false;
    processor.id_ex.RegWrite = false;
    processor.id_ex.MemToReg = false;

    processor.ex_mem.pc_imm = 0;
    processor.ex_mem.aluResult = 0;
    processor.ex_mem.regVal2 = 0;
    processor.ex_mem.rd = 0;
    processor.ex_mem.zero = false;
    processor.ex_mem.Branch = false;
    processor.ex_mem.MemRead = false;
    processor.ex_mem.MemWrite = false;
    processor.ex_mem.RegWrite = false;
    processor.ex_mem.MemToReg = false;

    processor.mem_wb.memData = 0;
    processor.mem_wb.aluResult = 0;
    processor.mem_wb.rd = 0;
    processor.mem_wb.RegWrite = false;
    processor.mem_wb.MemToReg = false;
}

void simulatePipeline(Processor &processor) {
    std::cout << "Starting pipeline simulation..." << std::endl;

    // Total pipeline stages
    const int PIPELINE_STAGES = 5;

    // Keep track of the number of cycles
    int cycle = 0;
    int decode_counter = 0;
    int execute_counter = 0;
    int memory_counter = 0;
    int writeback_counter = 0;
    int fetch_counter = 0;

    // Run until the pipeline is completely drained
    while (cycle < processor.memory.size() + PIPELINE_STAGES) {
        std::cout << "--------------------" << std::endl;
        std::cout << "Cycle: " << cycle + 1 << std::endl;

        // Stages executed in reverse order to ensure proper data flow
        if ((cycle>=4) && (writeback_counter < processor.memory.size())) {writeback_counter++; processor.writeBack();}      // WriteBack happens after 4 cycles
        if ((cycle>=3) && (memory_counter < processor.memory.size())) {memory_counter++; processor.memoryAccess();}   // MemoryAccess happens after 3 cycles
        if ((cycle>=2) && (execute_counter < processor.memory.size())) {execute_counter++; processor.execute();}        // Execute happens after 2 cycles
        if ((cycle>=1) && (decode_counter < processor.memory.size())) {decode_counter++; processor.decode();}        // Decode happens after 1 cycle
        if ((cycle>=0) && (fetch_counter < processor.memory.size())){fetch_counter++; processor.fetch();} // Fetch while instructions are left

        // Debug the program counter and pipeline state
        std::cout << "PC: " << processor.pc << std::endl;
        cycle++;
    }

    std::cout << "Pipeline simulation complete." << std::endl;
}
