#pragma once
#include <stdlib.h>
#include <jet_token.h>
#include <jet_da.h>

typedef struct jet_lexer
{
    size_t cursor;
    size_t len;
    size_t cur_line;
    size_t cur_col;
    const char* source;
    jet_da* token_da;
} jet_lexer;

bool jet_lexer_init(jet_lexer* lexer, const char* source, jet_da* token_da);
void jet_lexer_reset(jet_lexer* lexer);
void jet_lexer_dispose(jet_lexer* lexer);
bool jet_lexer_tokenize(jet_lexer* lexer);
























