#include <jet_semantics.h>
#include <jet_semantics_internals.h>

#include <jet_logger.h>
#include <jet_ast.h>
#include <jet_type.h>
#include <jet_da.h>
#include <jet_hs.h>
#include <jet_symbol.h>
#include <jet_scope.h>
#include <jet_ast_node.h>
#include <jet_func.h>

#define JET_SEMA_INIT_TYPE_CAP (32)
#define JET_SEMA_INIT_ERR_CAP (32)
#define JET_SEMA_INIT_SCOPE_CAP (32)
#define JET_SEMA_INIT_SYM_CAP (64)
#define JET_SEMA_INIT_FUNC_CAP (32)


// Public Interface #############################

bool jet_sema_init(jet_semantics* sema, jet_compilation_unit* cu)
{
    if(!cu || !sema) return false;
    
    if(
          !jet_ident_id_kv_hs_init(&cu->type_id_hs, JET_SEMA_INIT_TYPE_CAP)  
       || !jet_da_init(&cu->type_da, JET_SEMA_INIT_TYPE_CAP, sizeof(jet_type))  
       || !jet_da_init(&cu->sema_err_da, JET_SEMA_INIT_ERR_CAP, sizeof(jet_error)) 
       || !jet_da_init(&cu->scope_da, JET_SEMA_INIT_SCOPE_CAP, sizeof(jet_scope)) 
       || !jet_da_init(&cu->global_sym_da, JET_SEMA_INIT_SYM_CAP, sizeof(jet_symbol))
       || !jet_ident_id_kv_hs_init(&cu->global_symid_hs, JET_SEMA_INIT_SYM_CAP)
       || !jet_da_init(&cu->func_da, JET_SEMA_INIT_FUNC_CAP, sizeof(jet_func)) )  
        return false;

    *sema = (jet_semantics){0};
    sema->ast = &cu->ast;
    sema->type_da = &cu->type_da;
    sema->scope_da = &cu->scope_da;
    sema->global_sym_da = &cu->global_sym_da;
    sema->sema_err_da = &cu->sema_err_da;
    sema->type_id_hs = &cu->type_id_hs;
    sema->global_symid_hs = &cu->global_symid_hs;
    sema->func_da = &cu->func_da;

    if(!jet_da_init(&sema->custom_type_nid_da, JET_SEMA_INIT_TYPE_CAP, sizeof(size_t)))
        return false;

    return true;
}

void jet_sema_dispose(jet_semantics* sema)
{
    if(!sema) return;
    jet_da_dispose(&sema->custom_type_nid_da);
}

bool jet_sema_analyze(jet_semantics* sema)
{
    JET_ASSERT(sema != NULL);

    jet_sema_register_primitive_types(sema);
    jet_sema_register_custom_types(sema);
    JET_LOG_INFO("types registered");

    jet_sema_register_global_symbols(sema);
    JET_LOG_INFO("global symbols registered");

    jet_sema_resolve_type_data(sema);
    JET_LOG_INFO("types resolved");

    jet_sema_register_funcs(sema);
    JET_LOG_INFO("funcs registered");

    return true;
}



