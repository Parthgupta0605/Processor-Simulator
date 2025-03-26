#include "processor.hpp"
#include "forwarding.hpp"
#include "pipeline.hpp"
#include <vector>
#include <iostream>

int main() {
    // Example program (machine code instructions)
    std::vector<uint32_t> program = {
        0x00400093  ,     // addi x1 x0 4
        0x0000a023 ,      // sw x0 0 x1
        0x0000a103 ,      // lw x2 0 x1
        0x00110193  ,     // addi x3 x2 1
        0x00218463  ,     // beq x3 x2 8 <label>
        0x06400213        // addi x4 x0 100

    };

    // Initialize processor without forwarding
    std::cout << "Simulating Processor without forwarding:" << std::endl;
    Processor processor;
    initializePipeline(processor);
    std::cout << "Pipeline Initialized" << std::endl;
    processor.loadProgram(program);
    simulatePipeline(processor);

    // Initialize processor with forwarding
    // std::cout << "\nSimulating Processor with forwarding:" << std::endl;
    // ForwardingProcessor forwardingProcessor;
    // initializePipeline(forwardingProcessor);
    // forwardingProcessor.loadProgram(program);
    // simulatePipeline(forwardingProcessor);

    return 0;
}
