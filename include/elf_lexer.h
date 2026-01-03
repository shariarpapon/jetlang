#pragma once
#include <stdlib.h>
#include <elf_token.h>
#include <elf_utils.h>
#include <ctoolbox/tblist.h>

typedef struct
{
    const char* source;
    size_t len; //including null terminal ('\0')
    size_t cursor;
    tblist* tokens;
} elf_lexer;

elf_lexer* elf_lexer_create(const char* src);
void elf_lexer_full_dispose(elf_lexer* lexer);

BOOL elf_lexer_tokenize(elf_lexer* lexer);

BOOL elf_lexer_is_ident(char c);
BOOL elf_lexer_is_number(char c);
BOOL elf_lexer_is_whitespace(char c);

BOOL elf_lexer_scan_type_seq(elf_lexer* lexer, elf_token_type type, BOOL(*is_char_of_specified_type)(char));
BOOL elf_lexer_scan_line_com(elf_lexer* lexer);
BOOL elf_lexer_scan_block_com(elf_lexer* lexer);
BOOL elf_lexer_scan_opr(elf_lexer* lexer);
BOOL elf_lexer_scan_whitepace(elf_lexer* lexer);

elf_token_type elf_lexer_eval_opr_type(char c, BOOL* succ);
elf_token_type elf_lexer_eval_cmpd_opr_type(elf_token_type l, elf_token_type r, BOOL* succ);

char elf_lexer_consume(elf_lexer* lexer);
char elf_lexer_peek(elf_lexer* lexer);
char elf_lexer_peek_prev(elf_lexer* lexer);
char elf_lexer_peek_next(elf_lexer* lexer);
char elf_lexer_try_peek_ahead(elf_lexer* lexer, size_t n, BOOL* succ);

void elf_lexer_advance(elf_lexer* lexer);
void elf_lexer_emit_token(elf_lexer* lexer, size_t origin, size_t len, elf_token_type type);






















