#include <inttypes.h>

#include <jet_semantics_internals.h>
#include <jet_logger.h>

void jet_sema_register_global_symbols(jet_compilation_unit* cu)
{
    JET_ASSERT(cu != NULL);
}

size_t jet_sema_index_to_symid(size_t index)
{
    return index + 1;
}

size_t jet_sema_symid_to_index(size_t symid)
{
    JET_ASSERT(symid != JET_INVALID_SYMID);
    return symid - 1;
}
