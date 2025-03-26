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
    while (cycle < processor.memory.size() * PIPELINE_STAGES) {
        std::cout << "--------------------" << std::endl;
        std::cout << "Cycle: " << cycle + 1 << std::endl;
        switch (std::get<1>(processor.memory[processor.current]).stage) {
            case 5:
                if ((cycle>=4) && !(processor.wb_stall) && (writeback_counter < processor.memory.size())) {writeback_counter++; processor.writeBack();}      // WriteBack happens after 4 cycles
            case 4:
                if ((cycle>=3) && !(processor.mem_stall) && (memory_counter < processor.memory.size())) {memory_counter++; processor.memoryAccess();}   // MemoryAccess happens after 3 cycles
            case 3:
                if ((cycle>=2) && !(processor.ex_stall) && (execute_counter < processor.memory.size())) {execute_counter++; processor.execute();}        // Execute happens after 2 cycles
            case 2:
                if ((cycle>=1) && !(processor.id_stall) && (decode_counter < processor.memory.size())) {decode_counter++; processor.decode();}        // Decode happens after 1 cycle
            case 1:
                if ((cycle>=0) && !(processor.if_stall) && (fetch_counter < processor.memory.size())){fetch_counter++; processor.fetch();} // Fetch while instructions are left
            default :
                std::cout << "No instructions left to process." << std::endl;
                break;
        }

        // Debug the program counter and pipeline state
        std::cout << "PC: " << processor.pc << std::endl;
        cycle++;
        if (processor.mem_stall && processor.ex_stall && processor.id_stall && processor.if_stall) {
            processor.mem_stall = false;
            processor.ex_stall = false;
            processor.id_stall = false;
            processor.if_stall = false;
        }
        switch (std::get<1>(processor.memory[processor.current]).stage) {
            case 1:
            processor.if_stall = false;
            break;
            case 2:
            processor.id_stall = false;           
            break;
            case 3:
            processor.ex_stall = false;
            break;
            case 4:
            processor.mem_stall = false;
            break;
            case 5:
            processor.wb_stall = false;
            break;
            default:
                break;
        }
    }

    std::cout << "Pipeline simulation complete." << std::endl;
}
