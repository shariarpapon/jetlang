#include <string.h>
#include <jet_scope.h>
#include <jet_logger.h>

jet_scope jet_scope_init_base(size_t parent_scope_id)
{
    jet_scope scope = {0};
    scope.parent_scope_id = parent_scope_id;
    return scope;
}

void jet_scope_dispose(jet_scope* scope)
{
    if(!scope) return;
    jet_da_dispose(&scope->sym_da);
    memset(scope, 0,  sizeof(*scope));
}

