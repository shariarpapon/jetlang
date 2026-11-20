#include <jet_semantics_internals.h>
#include <jet_logger.h>
#include <jet_func.h>

void jet_sema_register_funcs(jet_semantics* sema)
{
    JET_ASSERT(sema != NULL);

    for(size_t i = 0; i < jet_da_count(sema->global_sym_da); i++)
    {
        jet_symbol* sym = (jet_symbol*)jet_da_get(sema->global_sym_da, i);
        if(sym->kind != JET_SYM_FUNC)
            continue;

    }
}

bool jet_sema_construct_func_from_node(jet_semantics* sema, size_t nid, jet_func* out_func)
{
    
}
