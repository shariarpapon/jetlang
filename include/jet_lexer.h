#pragma once
#include <jet_token.h>
#include <jet_da.h>
#include <jet_compilation_unit.h>

#include <stdlib.h>
#include <stdint.h>

typedef struct jet_lexer
{
    const char* filename;
    jet_da* token_da;
    size_t input_len;
    size_t cursor;
    uint32_t cur_line;
    uint32_t cur_col;
    const char* input;
} jet_lexer;

bool jet_lexer_init(jet_lexer* lexer, jet_compilation_unit* cu);
void jet_lexer_reset(jet_lexer* lexer);
void jet_lexer_dispose(jet_lexer* lexer);
bool jet_lexer_tokenize(jet_lexer* lexer);
























