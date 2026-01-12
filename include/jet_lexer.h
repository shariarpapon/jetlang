#pragma once
#include <stdlib.h>
#include <jet_token.h>
#include <jet_list.h>

typedef struct jet_lexer
{
    size_t len;
    size_t cursor;
    const char* source;
    jet_list* token_list;
} jet_lexer;


jet_lexer* jet_lexer_create(const char* src);
void jet_lexer_dispose(jet_lexer* lexer);
bool jet_lexer_tokenize(jet_lexer* lexer);
























