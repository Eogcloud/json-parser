CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude
DEBUG_FLAGS = -g -DDEBUG
RELEASE_FLAGS = -O2 -DNDEBUG

SRC_DIR = src
INCLUDE_DIR = include
TEST_DIR = tests
BUILD_DIR = build

SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Default target
all: debug

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Debug build
debug: CFLAGS += $(DEBUG_FLAGS)
debug: $(BUILD_DIR) $(BUILD_DIR)/libjson.a

# Release build  
release: CFLAGS += $(RELEASE_FLAGS)
release: $(BUILD_DIR) $(BUILD_DIR)/libjson.a

# Static library
$(BUILD_DIR)/libjson.a: $(OBJECTS)
	ar rcs $@ $^

# Object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Test program with Unity
test: debug
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) -DUNITY_INCLUDE_DOUBLE -o $(BUILD_DIR)/test_basic \
		$(TEST_DIR)/test_basic.c $(TEST_DIR)/unity/unity.c \
		-L$(BUILD_DIR) -ljson
	./$(BUILD_DIR)/test_basic

# Test objects specifically
test-objects: debug
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) -DUNITY_INCLUDE_DOUBLE -o $(BUILD_DIR)/test_objects \
		$(TEST_DIR)/test_objects.c $(TEST_DIR)/unity/unity.c \
		-L$(BUILD_DIR) -ljson
	./$(BUILD_DIR)/test_objects

# Clean
clean:
	rm -rf $(BUILD_DIR)

# Main program
json-parser: debug
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) -o $(BUILD_DIR)/json-parser src/main.c -L$(BUILD_DIR) -ljson

# Example program
example: debug
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) -o $(BUILD_DIR)/example examples/simple.c -L$(BUILD_DIR) -ljson

.PHONY: all debug release test clean example json-parser