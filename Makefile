# Compiler and flags
CC = gcc

HEADER_DIRS := $(shell find include utils libs -type d)
CFLAGS = -Wall -std=c11 -g $(addprefix -I,$(HEADER_DIRS))

PROJ_SRC_DIR = src
UTILS_SRC_DIR = utils
SRC_FILES = $(wildcard $(PROJ_SRC_DIR)/*.c $(UTILS_SRC_DIR)/*.c)

OBJ = $(patsubst %.c,build/%.o,$(SRC_FILES))
BIN = jet
LIBS := $(shell find libs -name "*.a")

JET_SRC_FILE_EXT = jt
JET_FLAGS =
TEST_EXEC = ./jet $(shell ls *.$(JET_SRC_FILE_EXT)) $(JET_FLAGS)

all: run

run: $(BIN)
	$(TEST_EXEC)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

build/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build $(BIN)
