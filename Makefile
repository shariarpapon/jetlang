# Compiler and flags
CC = gcc

HEADER_DIRS := $(shell find include libs -type d)
CFLAGS = -Wall -std=c11 -g $(addprefix -I,$(HEADER_DIRS))

SRC = $(wildcard src/*.c)
OBJ = $(SRC:src/%.c=build/%.o)
BIN = elf
ELF_CODE_FILE = *.ef pt

LIBS := $(shell find libs -name "*.a")

all: run

run: $(BIN)
	./elf $(ELF_CODE_FILE)

$(BIN): $(OBJ)
	mkdir -p build
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

build/%.o: src/%.c
	mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build $(BIN)

