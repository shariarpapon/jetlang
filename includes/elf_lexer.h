#pragma once
#include <stdlib.h>
#include <elf_token.h>
#include <toolbox/tblist.h>
#include <toolbox/tbdefines.h>

typedef struct ElfLexer
{
    const char* source;
    size_t len; //including null terminal ('\0')
    size_t cursor;
    tblist* tokens;
} elf_Lexer;

elf_Lexer* elf_Lexer_create(const char* src);

_bool elf_Lexer_tokenize(elf_Lexer* lexer);

_bool elf_Lexer_isIdentifier(char c);
_bool elf_Lexer_isNumber(char c);

char elf_Lexer_consume(elf_Lexer* lexer);
_bool elf_Lexer_consumeType(elf_Lexer* lexer, elf_TokenType type, _bool(*isMatch)(char));
_bool elf_Lexer_consumeLineComment(elf_Lexer* lexer);
_bool elf_Lexer_consumeBlockComment(elf_Lexer* lexer);

char elf_Lexer_peek(elf_Lexer* lexer);
char elf_Lexer_peekLast(elf_Lexer* lexer);
char elf_Lexer_peekNext(elf_Lexer* lexer);

void elf_Lexer_next(elf_Lexer* lexer);
void elf_Lexer_trimNext(elf_Lexer* lexer);
void elf_Lexer_emitToken(elf_Lexer* lexer, size_t origin, size_t len, elf_TokenType type);






















