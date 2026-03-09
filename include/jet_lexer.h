#pragma once
#include <jet_token.h>
#include <jet_da.h>

#include <stdlib.h>
#include <stdint.h>

typedef struct jet_lexer
{
    uint32_t cur_line;
    uint32_t cur_col;
    size_t cursor;
    size_t len;
    const char* source;
    jet_da* token_da;
} jet_lexer;

bool jet_lexer_init(jet_lexer* lexer, const char* source, jet_da* token_da);
void jet_lexer_reset(jet_lexer* lexer);
void jet_lexer_dispose(jet_lexer* lexer);
bool jet_lexer_tokenize(jet_lexer* lexer);
























