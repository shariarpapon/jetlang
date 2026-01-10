#pragma once
#include <stdlib.h>
#include <elf_token.h>
#include <elf_utils.h>
#include <ctoolbox/vec.h>

typedef struct elf_lexer
{
    size_t len;
    size_t cursor;
    const char* source;
    vec* token_vec;
} elf_lexer;


elf_lexer* elf_lexer_create(const char* src);
void elf_lexer_full_dispose(elf_lexer* lexer);
bool elf_lexer_tokenize(elf_lexer* lexer);
























