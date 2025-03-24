#ifndef PROCESSOR_HPP
#define PROCESSOR_HPP

#include "instruction.hpp"
#include <vector>
#include <cstdint>

class Processor {
public:
    Processor();
    void loadProgram(const std::vector<uint32_t>& program);
    void run();

    virtual void fetch();
    virtual void decode();
    virtual void execute();
    virtual void memoryAccess();
    virtual void writeBack();

    uint32_t registers[32];
    uint64_t pc;
    std::vector<uint32_t> memory;

    struct IF_ID {
        uint64_t pc;
        Instruction instr;
    } if_id;

    struct ID_EX {
        uint64_t pc;
        std::string opcode;
        int rd;
        int64_t imm;
        int rs1,rs2;
        int funct3, funct7;
        int regVal1, regVal2;

        bool ALUSrc;
        uint8_t ALUOp :2;
        bool Branch;
        bool MemRead;
        bool MemWrite;
        bool RegWrite;
        bool MemToReg;
    } id_ex;

    struct EX_MEM {
        uint64_t pc_imm;
        uint32_t aluResult;
        uint32_t regVal2;
        int rd;
        bool zero;

        bool Branch;
        bool MemRead;
        bool MemWrite;
        bool RegWrite;
        bool MemToReg;
    } ex_mem;

    struct MEM_WB {
        uint32_t memData;
        uint32_t aluResult;
        int rd;

        bool RegWrite;
        bool MemToReg;
    } mem_wb;


     
    Processor(bool enableForwarding);
};

#endif
