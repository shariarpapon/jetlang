#pragma once
#include <stdlib.h>
#include <toolbox/tblist.h>

typedef struct ElfLexer
{
    const char* source;
    tblist* tokens;
}elf_Lexer;

elf_Lexer* elf_Lexer_create(const char* src);





























