# Compiler and flags
CC = gcc

HEADER_DIRS := $(shell find include libs -type d)
CFLAGS = -Wall -std=c11 -g $(addprefix -I,$(HEADER_DIRS))

SRC = $(wildcard src/*.c)
OBJ = $(SRC:src/%.c=build/%.o)
BIN = jet
LIBS := $(shell find libs -name "*.a")

JET_FLAGS = pt
JET_CODE_FILE = *.jet
TEST_EXEC = ./jet $(JET_CODE_FILE) $(JET_FLAGS)

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

