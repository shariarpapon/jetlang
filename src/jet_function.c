#include <string.h>

#include <jet_function.h>
#include <jet_symbol.h>
#include <jet_logger.h>

#define JET_FUNC_SYM_DA_INIT_CAP (16)

jet_function jet_func_init_base()
{
    jet_function func;
    memset(&func, 0, sizeof(func));

    JET_ASSERT(jet_da_init(&func.symbol_da,JET_FUNC_SYM_DA_INIT_CAP, sizeof(jet_symbol)));
    return func;
}

void jet_func_dispose(jet_function* func)
{
    if(!func) return;
    jet_da_dispose(&func->symbol_da);
    memset(func, 0, sizeof(*func));
}



