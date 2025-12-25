#pragma once

#include "elf_defines.h"

typedef enum TokenType
{
    IDENTIFIER=0,
    RETURN,
    STRING_LITERAL

} t_elf_token_type;

typedef struct Token
{
    t_elf_token_type type;
    const char* source;
    size_t start;
    size_t end;

} t_elf_token;

typedef struct ElfLexer
{
    const char* source;
    t_elf_token** tokens;
}t_elf_lexer;

t_elf_lexer* elf_create_lexer(const char* source);
t_elf_token* elf_create_token(const char* source, size_t start, size_t end);
t_elf_token_type elf_get_tokentype(const char* source, size_t start, size_t end);



































