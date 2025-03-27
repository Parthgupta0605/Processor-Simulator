#ifndef PROCESSOR_HPP
#define PROCESSOR_HPP

#include "instruction.hpp"
#include <vector>
#include <tuple>
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
    virtual void PC_update();

    bool mem_stall = false;
    bool ex_stall = false;
    bool id_stall = false;
    bool if_stall = false;
    bool wb_stall = false;
    bool NOP=false ;
    uint32_t registers[32];
    int64_t pc;
    std::vector<std::tuple<uint32_t,struct Instruction>> memory;
    int current = -1;

    int64_t performALUOperation(int ALUOp, int64_t operand1, int64_t operand2, int funct3, int funct7);

    struct Entry {
        int PCsrc=0;
        uint64_t PC1;
        uint64_t PC2;
    } entry;

    struct IF_ID {
        uint64_t pc;
        Instruction instr;
    } if_id;

    struct ID_EX {
        Instruction instr;
        int64_t pc;
        std::string opcode;
        int rd;
        int64_t imm;
        int rs1,rs2;
        int funct3, funct7;
        int regVal1, regVal2;
        int returnAddress;

        bool ALUSrc;
        uint8_t ALUOp :2;
        bool PCsrc;
        bool Branch;
        bool MemRead;
        bool MemWrite;
        bool RegWrite;
        bool MemToReg;
    } id_ex;

    struct EX_MEM {
        Instruction instr;
        int64_t pc_imm,pc;
        uint32_t aluResult;
        uint32_t regVal2;
        int rs1,rs2;
        int rd;
        bool zero;
        int returnAddress;

        bool Branch;
        bool MemRead;
        bool MemWrite;
        bool RegWrite;
        bool MemToReg;
    } ex_mem;

    struct MEM_WB {
        Instruction instr;
        uint32_t memData;
        uint32_t aluResult;
        int64_t pc_imm,pc;
        int rd;
        int returnAddress;
        bool MemWrite;
        bool MemRead;
        bool RegWrite;
        bool MemToReg;
    } mem_wb;


    
};

#endif
