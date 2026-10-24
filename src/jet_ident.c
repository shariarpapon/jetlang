#include <jet_ident.h>
#include <jet_logger.h>
#include <string.h>

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
    return jet_hs_init(hs, capacity, jet_ident_hash, jet_ident_equals);
}

uint64_t jet_ident_hash(const void* ident_ptr)
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

bool jet_ident_equals(const void* ident_ptr_a, const void* ident_ptr_b)
{
    if(ident_ptr_a == NULL || ident_ptr_b == NULL)
    {
        JET_LOG_ERROR("attempeting to compare NULL pointer.");
        return false;
    }
    const jet_ident* a = ident_ptr_a;
    const jet_ident* b = ident_ptr_b;
    if(a->len != b->len) 
        return false;
    return memcmp(a->str, b->str, a->len) == 0;
}


