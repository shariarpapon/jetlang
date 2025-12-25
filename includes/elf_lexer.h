#pragma once

#include "elf_defines.h"

typedef enum TokenType
{
    INVALID_TOKEN=0,
    IDENTIFIER,
    DIGIT,
    OPERATOR,
    WHITE_SPACE,
    DOT,
    DB_QUOTE,
    STMT_END,
    SCOPE_BEG,
    SCOPE_END,
    EXPR_BEG,
    EXPR_END,
    TERMINATE,
} t_elf_token_type;

typedef struct Token
{
    char c;
    t_elf_token_type type;
} t_elf_token;

typedef struct ElfLexer
{
    const char* source;
    t_elf_token** tokens;
}t_elf_lexer;

t_elf_lexer* elf_create_lexer(const char* source);
t_elf_token* elf_create_token(char c, t_token_type type);
t_elf_token_type elf_get_tokentype(char c);



































