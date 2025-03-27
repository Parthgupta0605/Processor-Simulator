CXX = g++
CXXFLAGS = -std=c++11 -Wall

# Default target
all: noforward forward

# Compile without forwarding
noforward: main.o processor.o pipeline.o instruction.o
	$(CXX) $(CXXFLAGS) -o noforward main.o processor.o pipeline.o instruction.o

# Compile with forwarding (ensure processor.o is linked)
forward: main.o processor.o forwarding.o pipeline.o instruction.o
	$(CXX) $(CXXFLAGS) -DFORWARDING -o forward main.o processor.o forwarding.o pipeline.o instruction.o

# Compile main.cpp once (no need to duplicate for forwarding)
main.o: main.cpp processor.hpp forwarding.hpp pipeline.hpp instruction.hpp
	$(CXX) $(CXXFLAGS) -c main.cpp

processor.o: processor.cpp processor.hpp instruction.hpp
	$(CXX) $(CXXFLAGS) -c processor.cpp

forwarding.o: forwarding.cpp forwarding.hpp
	$(CXX) $(CXXFLAGS) -c forwarding.cpp

pipeline.o: pipeline.cpp pipeline.hpp instruction.hpp
	$(CXX) $(CXXFLAGS) -c pipeline.cpp

instruction.o: instruction.cpp instruction.hpp
	$(CXX) $(CXXFLAGS) -c instruction.cpp

# Clean up object files and executables
clean:
	rm -f *.o noforward forward
