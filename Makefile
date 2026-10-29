ifeq ($(OS),Windows_NT)
	DEL_FILE_CMD := cmd /C del /F /Q
	C_BIN := jet.exe
	RUN := $(C_BIN)
else
	DEL_FILE_CMD := rm -f
	C_BIN := jet
	RUN := ./$(C_BIN)
endif

CC = gcc
C_SRC_PATHS := src/*.c \
			   utils/*.c \
			   src/parser_internals/*.c

C_SRC := $(wildcard $(C_SRC_PATHS))

C_INCLUDES = -Iinclude -Iutils
C_FLAGS = -std=c11

C_COMPILE = $(CC) $(C_FLAGS) $(C_SRC) $(C_INCLUDES) -o $(C_BIN)  

jf ?=
JET_CODE_FILE = code.jt
JET_EXEC = $(RUN) $(JET_CODE_FILE)

.PHONY: all rebuild rb clean 

all:
	$(C_COMPILE)
	$(JET_EXEC) $(jf)

rebuild rb:
	$(MAKE) clean
	$(MAKE) all

clean:
	$(DEL_FILE_CMD) $(C_BIN)
