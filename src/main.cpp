#include "processor.hpp"
#include <vector>
#include <iostream>

int main() {
    Processor cpu;
    
    // Sample program: ADD x3, x1, x2; SUB x4, x1, x2
    std::vector<uint32_t> program = {0x002081B3, 0x402081B3};  
    cpu.loadProgram(program);

    cpu.run();

    return 0;
}
