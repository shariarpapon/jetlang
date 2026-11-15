#include <jet_semantics_internals.h>
#include <jet_logger.h>

void jet_sema_push_err(jet_compilation_unit* cu, const jet_error* err)
{
    JET_ASSERT(cu != NULL);
    JET_ASSERT(err != NULL);
    JET_ASSERT(jet_da_append(&cu->sema_err_da, (const void*)err));
}

void jet_sema_err_undefined_type(jet_compilation_unit* cu, const jet_ast_node* tspec)
{
    JET_ASSERT(cu != NULL);
    JET_ASSERT(tspec != NULL);
    jet_error err = jet_err_type_undef(tspec);
    jet_sema_push_err(cu, &err);
}

void jet_sema_err_dup_sym_name(jet_compilation_unit* cu, const jet_ast_node* node, jet_ident name)
{
    JET_ASSERT(cu != NULL);
    JET_ASSERT(node != NULL);
    JET_ASSERT(name.str != NULL && name.len != 0);
    jet_error err = jet_err_dup_sym_name(node, name);
    jet_sema_push_err(cu, &err);
}



