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

typedef struct ElfLexer
{
    const char* source;
    tblist* tokens;
}elf_Lexer;

elf_Lexer* elf_Lexer_create(const char* src);
elf_Token* elf_Token_create(const char* src, size_t origin, size_t len, elf_TokenType type);
elf_TokenType elf_TokenType_eval(char c);

void elf_TokenType_trim(const char* src, size_t* i, elf_TokenType trimType);
void elf_Lexer_generateTokens(elf_Lexer* lexer);
void elf_Lexer_addToken(elf_Lexer* lexer, size_t origin, size_t len, elf_TokenType type);































