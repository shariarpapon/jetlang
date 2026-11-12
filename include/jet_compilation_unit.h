#pragma once
#include <jet_arena.h>
#include <jet_ast.h>
#include <jet_hs.h>
#include <jet_da.h>

typedef struct jet_compilation_unit
{
    const char* filename;
    jet_arena arena;

    //lexer data
    const char* source;
    size_t source_len;
    jet_da tok_da;

    //parser data
    jet_ast ast;
    jet_da parser_err_da;
    jet_hs type_name_hs;

    //semantics data
    jet_da type_da;
    jet_hs type_id_hs;
    jet_da sema_err_da;
} jet_compilation_unit;

bool jet_cu_init(jet_compilation_unit* cu, const char* filename);
void jet_cu_dispose(jet_compilation_unit* cu);
bool jet_cu_run(jet_compilation_unit* cu);





