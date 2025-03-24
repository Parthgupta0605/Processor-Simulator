#ifndef FORWARDING_HPP
#define FORWARDING_HPP

#include "processor.hpp"

class ForwardingProcessor : public Processor {
public:
    ForwardingProcessor();
    void execute() override;
    void memoryAccess() override;

private:
    // Helper methods for forwarding logic
    bool checkForwardingEX_MEM_to_ID_EX(); // Forward EX/MEM to ID/EX
    bool checkForwardingMEM_WB_to_ID_EX(); // Forward MEM/WB to ID/EX
    bool checkForwardingEX_MEM_to_EX();    // Forward EX/MEM to EX
    bool checkForwardingMEM_WB_to_EX();    // Forward MEM/WB to EX

    // Forwarded values
    uint32_t forwardedVal1; // Forwarded value for operand 1
    uint32_t forwardedVal2; // Forwarded value for operand 2
};

#endif
