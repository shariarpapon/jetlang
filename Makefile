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
C_SRC := $(wildcard ./*.c src/*.c utils/*.c libs/*.c)
C_INCLUDES = -I. -Iinclude -Iutils -Ilibs
C_FLAGS = -Wall -std=c11

C_COMPILE = $(CC) $(C_FLAGS) $(C_SRC) $(C_INCLUDES) -o $(C_BIN)  

jf ?= pAll
JET_CODE_FILE = code.jt
JET_EXEC = $(RUN) $(JET_CODE_FILE)

.PHONY: all rebuild rb clean info_build ib

all:
	$(C_COMPILE)
	$(JET_EXEC) $(jf)

rebuild rb:
	$(MAKE) clean
	$(MAKE) all

clean:
	$(DEL_FILE_CMD) $(C_BIN)
