#include <jet_semantics_internals.h>
#include <jet_logger.h>

void jet_sema_push_err(jet_compilation_unit* cu, const jet_error* err)
{
    JET_ASSERT(cu != NULL);
    JET_ASSERT(err != NULL);
    JET_ASSERT(jet_da_append(&cu->sema_err_da, (const void*)err));
}









