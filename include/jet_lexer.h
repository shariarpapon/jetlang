#pragma once
#include <stdlib.h>
#include <jet_token.h>
#include <jet_da.h>

typedef struct jet_lexer
{
    size_t len;
    size_t cursor;
    size_t cur_line;
    const char* source;
    jet_da* token_darray;
} jet_lexer;

jet_lexer* jet_lexer_create(const char* src);
void jet_lexer_dispose(jet_lexer* lexer);
bool jet_lexer_tokenize(jet_lexer* lexer);
























