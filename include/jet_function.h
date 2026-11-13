#pragma once
#include <jet_da.h>

typedef struct jet_function
{
    size_t scope_id;
    jet_da symbol_da;
} jet_function;

jet_function jet_func_init_base();
void jet_func_dispose(jet_function* func);

 
