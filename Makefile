# Compiler and flags
CC = gcc

HEADER_DIRS := $(shell find include libs -type d)
CFLAGS = -Wall -std=c11 -g $(addprefix -I,$(HEADER_DIRS))

SRC = $(wildcard src/*.c)
OBJ = $(SRC:src/%.c=build/%.o)
BIN = elf
LIBS := $(shell find libs -name "*.a")

ELF_FLAGS =
ELF_CODE_FILE = *.ef
TEST_EXEC = ./elf $(ELF_CODE_FILE) $(ELF_FLAGS)

all: run

run: $(BIN)
	$(TEST_EXEC)

$(BIN): $(OBJ)
	mkdir -p build
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

build/%.o: src/%.c
	mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build $(BIN)

