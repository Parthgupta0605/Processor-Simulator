CXX = g++
CXXFLAGS = -std=c++11 -Wall

SRCS = main.cpp processor.cpp instruction.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = main

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS)

clean:
	rm -f $(OBJS) $(TARGET)
