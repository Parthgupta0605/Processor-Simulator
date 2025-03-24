# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

# Executable name
TARGET = processor_sim

# Source files
SRCS = main.cpp processor.cpp instruction.cpp forwarding.cpp pipeline.cpp

# Header files
HEADERS = processor.hpp instruction.hpp forwarding.hpp pipeline.hpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Default target
all: $(TARGET)

# Build the executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Compile source files into object files
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJS) $(TARGET)

# Run the simulation
run: $(TARGET)
	./$(TARGET)
