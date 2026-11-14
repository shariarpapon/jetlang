#pragma once
#include <jet_symbol.h>
#include <jet_da.h>

typedef struct jet_scope 
{
    jet_da sym_da;
} jet_scope;

jet_scope jet_scope_init_base();
void jet_scope_dispose(jet_scope* scope);







