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

void simulatePipeline(Processor &processor, int cycleCount) {
    std::cout << "Starting pipeline simulation..." << std::endl;

    // Keep track of the number of cycles
    int cycle = 0;
    unsigned int decode_counter = 0;
    unsigned int execute_counter = 0;
    unsigned int memory_counter = 0;
    unsigned int writeback_counter = 0;
    unsigned int fetch_counter = 0;

    // Run until the pipeline is completely drained
    while (cycle < cycleCount) {
        std::cout << "--------------------" << std::endl;
        std::cout << "Cycle: " << cycle + 1 << std::endl;
        switch (std::get<1>(processor.memory[processor.current]).stage) {
            case 5:
                if ((cycle>=4) && !(processor.wb_stall)  ) {
                    std::get<1>(processor.memory[writeback_counter]).cycleEntered[4] = cycle;
                    writeback_counter++; processor.writeBack();
                    // std::cout << "WriteBack" << std::endl;
                }      // WriteBack happens after 4 cycles
            case 4:
                if ((cycle>=3) && !(processor.mem_stall) ) {
                    std::get<1>(processor.memory[memory_counter]).cycleEntered[3] = cycle;
                    memory_counter++; processor.memoryAccess();
                    // std::cout << "MemoryAccess" << std::endl;
                }   // MemoryAccess happens after 3 cycles
            case 3:
                if ((cycle>=2) && !(processor.ex_stall) ) {
                    std::get<1>(processor.memory[execute_counter]).cycleEntered[2] = cycle;
                    execute_counter++; processor.execute();
                    // std::cout << "Execute" << std::endl;
                }        // Execute happens after 2 cycles
            case 2:
                if ((cycle>=1) && !(processor.id_stall) ) {
                    std::get<1>(processor.memory[decode_counter]).cycleEntered[1] = cycle;
                    decode_counter++; processor.decode();
                    // std::cout << "Decode" << std::endl;
                }        // Decode happens after 1 cycle
            case 1:
                if ((cycle>=0) && !(processor.if_stall) ){
                    std::get<1>(processor.memory[fetch_counter]).cycleEntered[0] = cycle;
                    fetch_counter++; processor.fetch();
                    // std::cout << "Fetch" << std::endl;
                } // Fetch while instructions are left
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
        std::cout<<processor.current<<std::endl;
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
    std::cout << "--------------------" << std::endl;
    for(unsigned int i=0; i < processor.memory.size(); i++){
        std::cout << "Instruction " << i + 1 << std::endl;
        std::cout << std::get<1>(processor.memory[i]).cycleEntered[0] << " " << std::get<1>(processor.memory[i]).cycleEntered[1] << " " << std::get<1>(processor.memory[i]).cycleEntered[2] << " " << std::get<1>(processor.memory[i]).cycleEntered[3] << " " << std::get<1>(processor.memory[i]).cycleEntered[4] << std::endl;
    }
}
