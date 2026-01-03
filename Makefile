# Compiler and flags
CC = gcc

# Recursively include all header directories under includes/ and libs/
HEADER_DIRS := $(shell find include libs -type d)
CFLAGS = -Wall -std=c11 -g $(addprefix -I,$(HEADER_DIRS))

# Source and build directories
SRC = $(wildcard src/*.c)
OBJ = $(SRC:src/%.c=build/%.o)
BIN = elf

# Automatically find all .a files in libs/
LIBS := $(shell find libs -name "*.a")

# Default target
all: $(BIN)

# Link object files with libraries
$(BIN): $(OBJ)
	mkdir -p build
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

# Compile source files into object files
build/%.o: src/%.c
	mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

# Custom target to clean and rebuild
rebuild rb:
	$(MAKE) clean
	$(MAKE) all

# Clean build artifacts
clean:
	rm -rf build $(BIN)
