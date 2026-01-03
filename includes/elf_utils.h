#pragma once
#include <stdlib.h>
#include <stdio.h>

#define elf_EOF '\0'
#define BOOL int
#define TRUE 1
#define FALSE 0

BOOL* elf_newBool();
void elf_disposeBool(BOOL* b);




























