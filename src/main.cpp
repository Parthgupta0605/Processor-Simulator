#include "processor.hpp"
#include "pipeline.hpp"
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#ifdef FORWARDING
#include "forwarding.hpp"
#endif

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_file> <cycle_count>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    int cycleCount = std::stoi(argv[2]);
    if (cycleCount <= 0) {
        std::cerr << "Error: Cycle count must be a positive integer!" << std::endl;
        return 1;
    }

    std::vector<uint32_t> program; // Vector to store machine codes

    // Open the input file
    std::ifstream inputFile(filename);
    if (!inputFile) {
        std::cerr << "Error: Could not open input file '" << filename << "'!" << std::endl;
        return 1;
    }

    std::string line;
    while (std::getline(inputFile, line)) {
        std::istringstream iss(line);
        std::string address, machineCode;

        // Read the address and machine code
        if (iss >> address >> machineCode) {
            uint32_t instruction = std::stoul(machineCode, nullptr, 16);
            program.push_back(instruction);
        }
    }

    inputFile.close();

#ifdef FORWARDING
    std::cout << "Simulating Processor **with** forwarding:" << std::endl;
    ForwardingProcessor processor;
#else
    std::cout << "Simulating Processor **without** forwarding:" << std::endl;
    Processor processor;
#endif

    initializePipeline(processor);
    std::cout << "Pipeline Initialized" << std::endl;
    processor.loadProgram(program);
    
    simulatePipeline(processor, cycleCount);

    return 0;
}
