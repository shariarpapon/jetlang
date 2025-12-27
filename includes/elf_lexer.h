#pragma once
#include <toolbox/tblist.h>

typedef enum TokenType
{
    TOK_INVALID,
    TOK_EOF,
    TOK_IDENT,
    TOK_WSPACE,
    TOK_KWORD,
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
    TOK_POUND,
    TOK_DOLLAR,
    TOK_CARET,
    TOK_DQUOTE,
    TOK_SQUOTE,
    TOK_BSLASH,
} elf_TokenType;

typedef struct Token
{
    const char* source;
    size_t origin;
    size_t len;
    elf_TokenType type;
} elf_Token;

typedef struct ElfLexer
{
    const char* source;
    tblist* tokens;
}elf_Lexer;

elf_Lexer* elf_Lexer_create(const char* src);
elf_Token* elf_Token_create(const char* src, size_t origin, size_t len, elf_TokenType type);
elf_TokenType elf_TokenType_eval(char c);
void elf_Lexer_generate_tokens(elf_Lexer* lexer);

































