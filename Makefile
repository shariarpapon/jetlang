# Compiler and flags
CC = gcc
CFLAGS = -Wall -std=c11 -g -Iincludes

# Source and build directories
SRC = $(wildcard src/*.c)
OBJ = $(SRC:src/%.c=build/%.o)
BIN = elf

# Default target
all: $(BIN)

# Link object files
$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

# Compile source files into object files
build/%.o: src/%.c
	mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf build $(BIN)

