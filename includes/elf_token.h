#pragma once

#include<stdlib.h>

typedef enum TokenType
{
    TOK_invalid,
    TOK_EOF,
    TOK_IDENT,
    TOK_keyword,
    TOK_WSPACE,
    TOK_NUMINT,
    TOK_NUMFLOAT,
    TOK_OPR,
    TOK_DOT,
    TOK_SCOLON,
    TOK_GTHAN,
    TOK_LTHAN,
    TOK_EQL,
    TOK_OBRACE,
    TOK_CBRACE,
    TOK_OPARAN,
    TOK_CPARAN,
    TOK_COMMA,
    TOK_BAR,
    TOK_AMP,
    TOK_DQUOTE,
    TOK_SQUOTE,
    TOK_FSLASH,
    TOK_BSLASH,
    TOK_DOLLAR,
    TOK_POUND,
    TOK_CARET,
} elf_TokenType;



typedef struct Token 
{
    const char* source;
    size_t origin;
    size_t len;
    elf_TokenType type;
} elf_Token;

elf_Token* elf_Token_create(const char* source, size_t origin, size_t len, elf_TokenType type);

elf_TokenType elf_TokenType_eval(char c);




