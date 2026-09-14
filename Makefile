# Compiler
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I./src

# Directories
SRC_DIR = src
APP_DIR = $(SRC_DIR)/application
NET_DIR = $(SRC_DIR)/networking
BUILD_DIR = build

# Target
TARGET = server

# Source files
SRCS = \
	$(SRC_DIR)/main.cpp \
	$(APP_DIR)/http_request.cpp \
	$(APP_DIR)/http_response.cpp \
	$(APP_DIR)/request_handler.cpp \
	$(NET_DIR)/socket.cpp

# Object files
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Default target
all: $(TARGET)

# Link
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# Rebuild
re: clean all

.PHONY: all clean re
