#pragma once
#include <stdlib.h>
#include <jet_token.h>
#include <jet_vector.h>

typedef struct jet_lexer
{
    size_t len;
    size_t cursor;
    const char* source;
    jet_vector* token_vec;
} jet_lexer;


jet_lexer* jet_lexer_create(const char* src);
void jet_lexer_dispose(jet_lexer* lexer);
bool jet_lexer_tokenize(jet_lexer* lexer);
























