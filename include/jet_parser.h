#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <jet_compilation_unit.h>

typedef struct jet_parser
{
    const char* filename;
    const jet_da* tok_da;
    jet_da* err_da;
    jet_ast* ast;
    jet_hs* type_name_hs;
    size_t tok_cursor;
} jet_parser;


bool jet_parser_init(jet_parser* p, jet_compilation_unit* cu);

bool jet_parser_dispose(jet_parser* p);
bool jet_parser_parse(jet_parser* p);

































