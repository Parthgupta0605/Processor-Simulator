#include "processor.hpp"
#include "forwarding.hpp"
#include "pipeline.hpp"
#include <vector>
#include <iostream>

int main() {
    // Example program (machine code instructions)
    std::vector<uint32_t> program = {
        // 0x00100093, // ADDI x1, x0, 1
        // 0x00208113, // ADDI x2, x1, 2
        0x002101B3, // ADD x3, x2, x2
        0x00410233  // ADD x4, x2, x4
    };

    // Initialize processor without forwarding
    std::cout << "Simulating Processor without forwarding:" << std::endl;
    Processor processor;
    initializePipeline(processor);
    std::cout << "Pipeline Initialized" << std::endl;
    processor.loadProgram(program);
    simulatePipeline(processor);

    // Initialize processor with forwarding
    std::cout << "\nSimulating Processor with forwarding:" << std::endl;
    ForwardingProcessor forwardingProcessor;
    initializePipeline(forwardingProcessor);
    forwardingProcessor.loadProgram(program);
    simulatePipeline(forwardingProcessor);

    return 0;
}
