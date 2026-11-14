#include <string.h>
#include <jet_scope.h>
#include <jet_logger.h>

#define JET_SCOPE_INIT_SYM_CAP (16)

jet_scope jet_scope_init_base()
{
    jet_scope scope = {0};
    JET_ASSERT(jet_da_init(&scope.sym_da, JET_SCOPE_INIT_SYM_CAP, sizeof(scope)));
    return scope;
}

void jet_scope_dispose(jet_scope* scope)
{
    if(!scope) return;
    jet_da_dispose(&scope->sym_da);
    memset(scope, 0,  sizeof(*scope));
}

