CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

# Source files
SRC_NON_FORWARDING = main_non_forwarding.cpp processor.cpp pipeline.cpp instruction.cpp
SRC_FORWARDING = main_forwarding.cpp forwarding.cpp processor.cpp pipeline.cpp instruction.cpp

# Object files
OBJ_NON_FORWARDING = $(SRC_NON_FORWARDING:.cpp=.o)
OBJ_FORWARDING = $(SRC_FORWARDING:.cpp=.o)

# Executables
EXE_NON_FORWARDING = noforward
EXE_FORWARDING = forward

# Directories
PARENT_DIR = ..
INPUT_DIR = $(PARENT_DIR)/inputfiles
OUTPUT_DIR = $(PARENT_DIR)/outputfiles

# Targets
all: $(EXE_NON_FORWARDING) $(EXE_FORWARDING)
	mkdir -p $(OUTPUT_DIR)
	for input_file in $(INPUT_DIR)/*; do \
		filename=$$(basename $$input_file); \
		output_base=$$(basename $$input_file .txt); \
		./$(EXE_NON_FORWARDING) $$input_file 30 > $(OUTPUT_DIR)/$${output_base}_noforward_out.txt; \
		./$(EXE_FORWARDING) $$input_file 30 > $(OUTPUT_DIR)/$${output_base}_forward_out.txt; \
	done

$(EXE_NON_FORWARDING): $(OBJ_NON_FORWARDING)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(EXE_FORWARDING): $(OBJ_FORWARDING)
	$(CXX) $(CXXFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ_NON_FORWARDING) $(OBJ_FORWARDING) $(EXE_NON_FORWARDING) $(EXE_FORWARDING)
	rm -rf $(OUTPUT_DIR)
