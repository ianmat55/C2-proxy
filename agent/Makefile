# Makefile for compiling C++ files with clang and C++17

# Compiler and compiler flags
CXX = clang++
CXXFLAGS = -std=c++20

# List of source files and corresponding object files
SRCS = main.cpp implant.cpp tasks.cpp
OBJS = $(SRCS:.cpp=.o)

# The final executable name
TARGET = agent

# Default target to build the executable
all: $(TARGET)

# Target to link the object files and create the executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Target to compile each individual source file into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Target to clean up the generated object files and the executable
clean:
	rm -f $(OBJS) $(TARGET)
