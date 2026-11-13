#include <inttypes.h>

#include <jet_semantics_internals.h>
#include <jet_logger.h>

typedef struct jet_sym_hs_entry
{
    jet_ident name;
    size_t sym_id;
} jet_sym_hs_entry;

static bool jet_sema_is_sym_hs_entry_equal(const void* entry_a, const void* entry_b);
static uint64_t jet_sema_sym_hs_entry_hash(const void* entry);

bool jet_sema_init_symbol_id_hs(jet_hs* hs, size_t capacity)
{
    JET_ASSERT(hs != NULL);
    JET_ASSERT(capacity > 0);
    return jet_hs_init(hs, capacity, jet_sema_sym_hs_entry_hash, jet_sema_is_sym_hs_entry_equal);
}

void jet_sema_register_global_symbols(jet_compilation_unit* cu)
{
    JET_ASSERT(cu != NULL);
}

static bool jet_sema_is_sym_hs_entry_equal(const void* entry_a, const void* entry_b)
{
    const jet_sym_hs_entry* a = (const jet_sym_hs_entry*)entry_a;
    const jet_sym_hs_entry* b = (const jet_sym_hs_entry*)entry_b;
    return jet_ident_value_equals(&a->name, &b->name);
}

static uint64_t jet_sema_sym_hs_entry_hash(const void* entry)
{
    const jet_sym_hs_entry* e = (const jet_sym_hs_entry*)entry;
    return jet_ident_hash(&e->name);
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
