#include <string.h>

#include <jet_func.h>
#include <jet_symbol.h>
#include <jet_logger.h>

#define JET_FUNC_SIG_INIT_PARAM_DA_CAP (8)

jet_func jet_func_init_base(size_t ret_type_id)
{
    jet_func func = {0};
    func.sig = jet_func_signature_init_base(ret_type_id);
    return func;
}

void jet_func_dispose(jet_func* func)
{
    if(!func) return;
    jet_func_signature_dispose(&func->sig);
    memset(func, 0, sizeof(*func));
}

jet_func_signature jet_func_signature_init_base(size_t ret_type_id)
{
    jet_func_signature sig = {0};
    sig.ret_type_id = ret_type_id;
    JET_ASSERT(jet_da_init(&sig.param_sym_da, JET_FUNC_SIG_INIT_PARAM_DA_CAP, sizeof(jet_symbol)));
    return sig;
}

void jet_func_signature_dispose(jet_func_signature* sig)
{
    if(!sig) return;
    jet_da_dispose(&sig->param_sym_da);
    memset(sig, 0, sizeof(*sig));
}

