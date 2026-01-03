#pragma once
#include <stdlib.h>
#include <elf_token.h>
#include <elf_utils.h>
#include <toolbox/tblist.h>

typedef struct ElfLexer
{
    const char* source;
    size_t len; //including null terminal ('\0')
    size_t cursor;
    tblist* tokens;
} elf_Lexer;

elf_Lexer* elf_Lexer_create(const char* src);
void elf_Lexer_fullDispose(elf_Lexer* lexer);

BOOL elf_Lexer_tokenize(elf_Lexer* lexer);

BOOL elf_Lexer_isIdentifier(char c);
BOOL elf_Lexer_isNumber(char c);
BOOL elf_Lexer_isWhiteSpace(char c);

BOOL elf_Lexer_consumeTypeSeq(elf_Lexer* lexer, elf_TokenType type, BOOL(*isMatch)(char));
BOOL elf_Lexer_consumeLineComment(elf_Lexer* lexer);
BOOL elf_Lexer_consumeBlockComment(elf_Lexer* lexer);
BOOL elf_Lexer_consumeOperator(elf_Lexer* lexer);
BOOL elf_Lexer_consumeWhiteSpace(elf_Lexer* lexer);

elf_TokenType elf_Lexer_tryGetSingleCharOprType(char c, BOOL* succ);
elf_TokenType elf_Lexer_tryGetCompoundOprType(elf_TokenType l, elf_TokenType r, BOOL* succ);

char elf_Lexer_consume(elf_Lexer* lexer);
char elf_Lexer_peek(elf_Lexer* lexer);
char elf_Lexer_peekLast(elf_Lexer* lexer);
char elf_Lexer_peekNext(elf_Lexer* lexer);
char elf_Lexer_tryPeekAhead(elf_Lexer* lexer, size_t n, BOOL* succ);

void elf_Lexer_advance(elf_Lexer* lexer);
void elf_Lexer_emitToken(elf_Lexer* lexer, size_t origin, size_t len, elf_TokenType type);






















