#pragma once
#include <stdbool.h>
#include <jet_compilation_unit.h>

typedef struct jet_semantics
{
    const jet_ast* ast;
    jet_da* type_da;
    jet_da* global_sym_da;
    jet_da* sema_err_da;
    jet_hs* type_id_hs;
    jet_hs* global_symid_hs;
    jet_da* func_da;
    
    jet_da* scope_da;
    jet_hs* scope_id_hs;

    jet_da custom_type_nid_da;
} jet_semantics;

bool jet_sema_init(jet_semantics* sema, jet_compilation_unit* cu);
void jet_sema_dispose(jet_semantics* sema);
bool jet_sema_analyze(jet_semantics* sema);

