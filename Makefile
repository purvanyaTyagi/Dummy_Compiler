# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -std=c++17

# Directories
SRC_DIR = src
BUILD_DIR = build

# Sources and object files
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

# Output binary
TARGET = $(BUILD_DIR)/main

# Default target
all: $(TARGET)

# Linking the final executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compiling source files to object files in build/
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up
clean:
	rm -rf $(BUILD_DIR)/*

# Test input file and test target
TEST_INPUT = $(shell cat tests/input.txt)

test: $(TARGET)
	@$(TARGET) "$(TEST_INPUT)"


.PHONY: all clean
