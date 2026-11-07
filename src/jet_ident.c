#include <jet_ident.h>
#include <jet_logger.h>
#include <string.h>

#define JET_CMP_INPUT_VALID(a,b) \
        ((a) != NULL && (b) != NULL && \
        ((a)->len == 0 || (a)->str != NULL) && \
        ((b)->len == 0 || (b)->str != NULL))

static bool jet_ident_generic_equals(const void* ident_ptr_a, const void* ident_ptr_b);
static uint64_t jet_ident_generic_hash(const void* ident_tpr);

jet_ident jet_ident_init(const char* str, size_t len)
{
    jet_ident ident = {.str = str, .len = len};
    return ident;
}

const jet_ident* jet_ident_hashset_add(jet_hs* hs, jet_ident ident)
{
    JET_ASSERT(hs != NULL);
    JET_ASSERT(ident.str != NULL);
    const void* stored = jet_hs_add(hs, &ident, sizeof(ident));
    if(stored == NULL)
    {
        JET_LOG_ERROR("failed to add to ident to hashset."); 
        return NULL;
    }
    return (const jet_ident*)stored;
}

bool jet_ident_hashset_init(jet_hs* hs, size_t capacity)
{
    JET_ASSERT(hs != NULL);
    return jet_hs_init(hs, capacity, jet_ident_generic_hash, jet_ident_generic_equals);
}

bool jet_ident_value_equals(const jet_ident* a, const jet_ident* b)
{
    if(!JET_CMP_INPUT_VALID(a,b)) 
    {
        JET_LOG_ERROR("invalid input/s.");
        return false;
    }
    if(a->len != b->len)
        return false;
    if(a->len == 0) 
        return true;
    return memcmp(a->str, b->str, a->len) == 0;
}

uint64_t jet_ident_hash(const jet_ident* ident_ptr)
{
    return jet_ident_generic_hash((const void*)ident_ptr);
}

static bool jet_ident_generic_equals(const void* ident_ptr_a, const void* ident_ptr_b)
{
    return jet_ident_value_equals((const jet_ident*)ident_ptr_a, (const jet_ident*)ident_ptr_b);
}

static uint64_t jet_ident_generic_hash(const void* ident_ptr)
{
    if(ident_ptr == NULL)
    {
        JET_LOG_ERROR("attempeting to hash NULL pointer.");
        return 0;
    }
    const jet_ident* ident = ident_ptr;
    uint64_t hash = 14695981039346656037ULL;
    for(size_t i = 0; i < ident->len; i++)
    {
        hash ^= (uint8_t)ident->str[i];
        hash *= 1099511628211ULL;
    }
    return hash;
}



