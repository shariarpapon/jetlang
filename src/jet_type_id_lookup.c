#include <inttypes.h>
#include <jet_type_id_lookup.h>
#include <jet_logger.h>

typedef struct jet_tid_entry
{
    jet_ident type_name;
    size_t type_id;
} jet_tid_entry;

static uint64_t jet_tid_hs_hash(const void* entry);
static bool jet_tid_hs_is_key_equal(const void* entry_a, const void* entry_b);

bool jet_tid_hs_init(jet_hs* hs, size_t capacity)
{
    if(hs == NULL || capacity == 0) 
        return false;
    return jet_hs_init(hs, capacity, jet_tid_hs_hash, jet_tid_hs_is_key_equal);
}

bool jet_tid_hs_add(jet_hs* hs, jet_ident type_name, size_t type_id)
{
    if(jet_tid_hs_has_name(hs, type_name))
        return false;

    jet_tid_entry key = {.type_name = type_name, .type_id = type_id};
    return jet_hs_add(hs, (const void*)&key, sizeof(key)) != NULL;
}

bool jet_tid_hs_has_name(const jet_hs* hs, jet_ident type_name)
{
    JET_ASSERT(hs != NULL);
    return jet_tid_hs_get_id(hs, type_name) != JET_INVALID_TID;
}

size_t jet_tid_hs_get_id(const jet_hs* hs, jet_ident type_name)
{
    JET_ASSERT(hs != NULL);
    jet_tid_entry temp = {0};
    temp.type_name = type_name; 
    const void* out = NULL;
    if(jet_hs_contains(hs, (const void*)&temp, &out))
    {
        temp = *((jet_tid_entry*)out);
        return temp.type_id;
    }
    return JET_INVALID_TID;
}

static uint64_t jet_tid_hs_hash(const void* entry)
{
    JET_ASSERT(entry != NULL);
    const jet_tid_entry* e = entry;
    return jet_ident_hash(&e->type_name);
}

static bool jet_tid_hs_is_key_equal(const void* entry_a, const void* entry_b)
{
    JET_ASSERT(entry_a && entry_b);
    const jet_tid_entry* a = entry_a; 
    const jet_tid_entry* b = entry_b;
    return jet_ident_value_equals(&a->type_name, &b->type_name);
}



