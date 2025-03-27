# COL216-ASS2
# Processor Simulator - README

## Design Decisions
### 1. **Single Makefile for Both Executables**
Instead of maintaining separate `Makefile` entries for `noforward` and `forward`, I designed a unified `Makefile` that:
- Compiles `main.cpp` only once.
- Uses the `-DFORWARDING` flag to differentiate `forward`.
- Ensures modular compilation by including necessary `.o` files.

### 2. **Abstract Base Class (`Processor`) and Derived Class (`ForwardingProcessor`)**
- The `Processor` class provides a **base implementation** for common functionality (e.g., `fetch()`, `decode()`, `execute()`, etc.).
- `ForwardingProcessor` **inherits from** `Processor` and overrides necessary methods to enable forwarding.
- This approach promotes **code reuse** and avoids redundant implementations.

### 3. **Instruction Representation**
- Instructions are stored in a `std::vector<std::tuple<uint32_t, Instruction>> memory;`.
- The `Instruction` class encapsulates instruction decoding, ensuring separation of concerns between execution and representation.

### 4. **Pipeline Stages Using Structs**
Each pipeline stage (`IF_ID`, `ID_EX`, `EX_MEM`, `MEM_WB`) is implemented as a struct with necessary fields to maintain state between stages.
- These structures help **mimic actual processor pipeline registers**.

### 7. **Handling Pipeline Stalls**

- Implemented explicit stall signals (mem_stall, ex_stall, id_stall, if_stall, wb_stall) to manage instruction execution delays.
- The control logic decides when to introduce stalls and when to resume execution.
- This design improves accuracy in simulating a real CPU pipeline while keeping the implementation structured and readable.

### 5. **Memory Representation and PC Management**
- Memory is implemented as a vector of tuples, where each entry contains an instruction and its corresponding program counter (PC).
- The `Processor` class maintains a **program counter (`pc`)** to track execution.

## Known Issues
### 1. **Sign-Unsigned Integer Comparison Warning**
- `pipeline.cpp:60:18: warning: comparison of integer expressions of different signedness`.
- This happens due to comparing an `unsigned int` (vector size) with an `int` (cycle count).
- Could be fixed by ensuring both variables are of the same type (`size_t` or `unsigned int`).

### 2. **Undefined Reference to `Processor` Methods**
- Initially, linking errors occurred due to missing implementations in `processor.cpp`.
- This was resolved by ensuring all declared methods in `processor.hpp` had definitions in `processor.cpp`.

### 3. **Incorrect PC Updates in Some Cases**
- Conditional branching may cause incorrect PC updates.
- Needs more testing to ensure correct branching behavior.

## Sources Consulted
### 1. **ChatGPT**
- Used to debug compilation issues (e.g., "undefined reference" errors).
- Provided explanations for `Makefile` structuring and linking.
- Suggested proper virtual function handling in `Processor`.
- Used for debugging the code at various stages by generating the main file for it.

### 2. **COL216 Lecture Notes**
- Reference for MIPS pipeline stages and forwarding logic.
- Used to ensure correct structuring of `Processor`.

### 3. **Stack Overflow**
- [Fixing Undefined Reference Errors](https://stackoverflow.com/questions/undefined-reference-c)
- [Handling Virtual Functions in C++](https://stackoverflow.com/questions/cpp-virtual-function)

### 4. **Textbook**
- [Hennessy and Patterson, Computer Organization](http://www.cse.iitd.ac.in/~rijurekha/col216_2025/HandP_RISCV.pdf)
---
