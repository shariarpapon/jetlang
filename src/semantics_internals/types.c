#include <jet_semantics_internals.h>
#include <jet_logger.h>

typedef struct jet_type_entry
{
    jet_ident type_name;
    size_t type_id;
} jet_type_entry;

static uint64_t jet_sema_type_entry_hash(const void* entry);
static bool jet_sema_is_type_entry_equal(const void* entry_a, const void* entry_b);

bool jet_sema_init_type_id_hs(jet_hs* hs, size_t capacity)
{
    if(hs == NULL || capacity == 0) 
        return false;
    return jet_hs_init(hs, capacity, jet_sema_type_entry_hash, jet_sema_is_type_entry_equal);
}

bool jet_sema_add_type_id(jet_hs* hs, jet_ident type_name, size_t type_id)
{
    if(jet_sema_has_type_name(hs, type_name))
        return false;

    jet_type_entry key = {.type_name = type_name, .type_id = type_id};
    return jet_hs_add(hs, (const void*)&key, sizeof(key)) != NULL;
}

bool jet_sema_has_type_name(const jet_hs* hs, jet_ident type_name)
{
    JET_ASSERT(hs != NULL);
    return jet_sema_get_type_id(hs, type_name) != JET_INVALID_TID;
}

size_t jet_sema_get_type_id(const jet_hs* hs, jet_ident type_name)
{
    JET_ASSERT(hs != NULL);
    jet_type_entry temp = {0};
    temp.type_name = type_name; 
    const void* out = NULL;
    if(jet_hs_contains(hs, (const void*)&temp, &out))
    {
        temp = *((jet_type_entry*)out);
        return temp.type_id;
    }
    return JET_INVALID_TID;
}

size_t jet_sema_register_typedef(jet_compilation_unit* cu, jet_typedef def)
{
    JET_ASSERT(cu != NULL);
    JET_ASSERT(jet_da_append(&cu->typedef_da, &def));
    return jet_da_count(&cu->typedef_da);
}

size_t jet_sema_tid_from_index(size_t index)
{
    return index + 1;
}

size_t jet_sema_index_from_tid(size_t tid)
{
    JET_ASSERT(tid > 0);
    return tid - 1;
}

static uint64_t jet_sema_type_entry_hash(const void* entry)
{
    JET_ASSERT(entry != NULL);
    const jet_type_entry* e = entry;
    return jet_ident_hash(&e->type_name);
}

static bool jet_sema_is_type_entry_equal(const void* entry_a, const void* entry_b)
{
    JET_ASSERT(entry_a && entry_b);
    const jet_type_entry* a = entry_a; 
    const jet_type_entry* b = entry_b;
    return jet_ident_value_equals(&a->type_name, &b->type_name);
}
