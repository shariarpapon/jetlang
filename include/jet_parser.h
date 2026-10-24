#pragma once
#include <jet_token.h>
#include <jet_ast.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct jet_parser
{
    const char* filename;
    const jet_da* tok_da;
    jet_da* err_da;
    jet_da* user_type_da;
    jet_ast* ast;
    size_t tok_cursor;
} jet_parser;


bool jet_parser_init(
        jet_parser* p, 
        const char* filename, 
        const jet_da* tok_da, 
        jet_ast* out_ast, 
        jet_da* err_da,
        jet_hs* tdef_name_hs);

bool jet_parser_dispose(jet_parser* p);
bool jet_parser_parse(jet_parser* p);

































