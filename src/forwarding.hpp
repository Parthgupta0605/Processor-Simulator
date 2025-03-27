#ifndef FORWARDING_HPP
#define FORWARDING_HPP

#include "processor.hpp"

class ForwardingProcessor : public Processor {
public:
    ForwardingProcessor();
    void execute() override;
    void memoryAccess() override;
    void writeBack() override;

private:
    // Forwarded values
    uint32_t forwardedVal1; // Forwarded value for operand 1
    uint32_t forwardedVal2; // Forwarded value for operand 2
};

#endif
