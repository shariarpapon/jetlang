#pragma once
#include <stdlib.h>
#include <elf_token.h>
#include <elf_utils.h>
#include <ctoolbox/vec.h>

typedef struct
{
    const char* source;
    size_t len; //including null terminal ('\0')
    size_t cursor;
    bool emit_comments;
    vec* tokens;
} elf_lexer;

elf_lexer* elf_lexer_create(const char* src);
void elf_lexer_full_dispose(elf_lexer* lexer);

bool elf_lexer_tokenize(elf_lexer* lexer);

bool elf_lexer_is_ident(char c);
bool elf_lexer_is_digit(char c);
bool elf_lexer_is_whitespace(char c);

bool elf_lexer_try_scan_ident(elf_lexer* lexer);
bool elf_lexer_try_scan_num(elf_lexer* lexer);
bool elf_lexer_try_scan_line_com(elf_lexer* lexer);
bool elf_lexer_try_scan_block_com(elf_lexer* lexer);
bool elf_lexer_try_scan_char(elf_lexer* lexer);
bool elf_lexer_try_scan_whitespace(elf_lexer* lexer);

bool elf_lexer_try_get_kwd_type(const char* s, size_t len, elf_token_type* out_tok_type);
bool e_lexer_try_get_char_type(char c, elf_token_type* out_tok_type);
bool e_lexer_try_get_cmpd_char_type(char lhs, char rhs, elf_token_type* out_tok_type);

elf_token_type elf_lexer_try_get_char_type(char c, bool* succ);
elf_token_type elf_lexer_try_get_cmpd_char_type(elf_token_type l, elf_token_type r, bool* succ);

char elf_lexer_consume(elf_lexer* lexer);
char elf_lexer_peek(elf_lexer* lexer);
char elf_lexer_peek_prev(elf_lexer* lexer);
char elf_lexer_peek_next(elf_lexer* lexer);
char elf_lexer_try_peek_ahead(elf_lexer* lexer, size_t n, bool* succ);

void elf_lexer_advance(elf_lexer* lexer);
void elf_lexer_emit_token(elf_lexer* lexer, size_t origin, size_t len, elf_token_type type);






















