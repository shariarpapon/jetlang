#pragma once

#include <jet_token.h>
#include <stdbool.h>
#include <stddef.h>
#include <jet_ast.h>

typedef struct jet_parse
{
    jet_da* tok_da;
    jet_ast* ast;
    size_t tok_cursor;
} jet_parser;


bool jet_parser_init(jet_parser* p, jet_da* tok_da, jet_ast* out_ast);
bool jet_parser_dispose(jet_parser* p);
bool jet_parser_parse(jet_parser* p);

































