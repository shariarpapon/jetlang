#pragma once
#include <jet_ast.h>
#include <jet_lexer.h>
#include <jet_parser.h>
#include <jet_token.h>
#include <jet_arena.h>

typedef struct jet_compilation_unit
{
    const char* filename;
    const char* source;
    size_t source_len;
    jet_arena arena;
    jet_da tok_da;
    jet_ast ast;
} jet_compilation_unit;

bool jet_cu_init(jet_compilation_unit* cu, const char* filename);
void jet_cu_dispose(jet_compilation_unit* cu);
bool jet_cu_run(jet_compilation_unit* cu);





