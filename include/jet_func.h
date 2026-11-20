#pragma once
#include <jet_da.h>

typedef struct jet_func_signature
{
    size_t ret_type_id;
    jet_da param_sym_da;
} jet_func_signature;


typedef struct jet_func
{
    jet_func_signature sig;
    size_t scope_id;
} jet_func;


jet_func jet_func_init_base();
void jet_func_dispose(jet_func* func);


jet_func_signature jet_func_sig_init_base();
void jet_func_sig_dispose(jet_func_signature* sig);
 
