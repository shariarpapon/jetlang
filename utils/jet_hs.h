// Shariar Papon - 21 October 2026
//
// - Hashset with seperate chaining
// - No auto resizing/re-allocation
// - Safe to free source key once added, as the hashset allocates its own copy.

#pragma once
#include <stdbool.h>
#include <inttypes.h>
#include <stddef.h>

typedef struct jet_hs jet_hs;

bool jet_hs_init(
        jet_hs* hs,
        size_t capacity,
        uint64_t (*hash) (const void*),
        bool (*equals) (const void*, const void*));

void jet_hs_dispose_full(jet_hs* hs);
void jet_hs_dispose_preserve_keys(jet_hs* hs);
bool jet_hs_contains(jet_hs* hs, const void* key, const void** out_key);

//returns: the address the key was allocated at or the key that was 
const void* jet_hs_add(jet_hs* hs, const void* key, size_t key_size);
bool jet_hs_remove(jet_hs* hs, const void* key);

#ifdef JET_HT_IMPL

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

typedef struct jet_hs_slot
{
    void* key;
    struct jet_hs_slot* next;
} jet_hs_slot;

struct jet_hs
{
    jet_hs_slot* slots;
    size_t capacity;
    size_t count;

    uint64_t (*hash) (const void*);
    bool (*equals) (const void*, const void*);
};

static bool jet_hs_invalid(jet_hs* hs);
static bool jet_hs_slot_fill(jet_hs_slot* slot, const void* key, size_t key_size);
static void jet_hs_slot_dispose_all(jet_hs_slot* slot, bool preserve_key);

static size_t jet_hs_calculate_index(jet_hs* hs, const void* key);
static jet_hs_slot* jet_hs_get_available_slot(jet_hs* hs, size_t index);
static jet_hs_slot* jet_hs_slot_at(jet_hs* hs, size_t index);
static jet_hs_slot* jet_hs_slot_with_key(jet_hs* hs, const void* key);

static bool jet_hs_invalid(jet_hs* hs)
{
    if(hs == NULL)
    {
        fprintf(stderr, "hashset pointer is null.\n");
        return true;
    }
    if(hs->slots == NULL        || 
            hs->capacity == 0   || 
            hs->hash == NULL    || 
            hs->equals == NULL  || 
            hs->count >= hs->capacity)
    {
        fprintf(stderr, "hashset in invalid state, make sure to initialize.\n");
        return true;
    }
    return false;
}

static size_t jet_hs_calculate_index(jet_hs* hs, const void* key)
{
    if(jet_hs_invalid(hs)) 
        return 0;
    if(key == NULL)
    {
        fprintf(stderr, "key is null.\n");
        return 0;
    }

    uint64_t hash = hs->hash(key);
    return hash % hs->capacity;
}

static jet_hs_slot* jet_hs_get_available_slot(jet_hs* hs, size_t index)
{
    if(jet_hs_invalid(hs))
        return NULL;

    jet_hs_slot* slot_i = jet_hs_slot_at(hs, index);
    assert(slot_i != NULL); //root slot should never be null post-init.

    while(slot_i->key != NULL && slot_i->next != NULL)
        slot_i = slot_i->next;

    if(slot_i->key == NULL)
        return slot_i;
    else
    {
        slot_i->next = (jet_hs_slot*)calloc(1, sizeof(jet_hs_slot));
        return slot_i->next;
    }
}

static jet_hs_slot* jet_hs_slot_at(jet_hs* hs, size_t index)
{
    if(jet_hs_invalid(hs))
        return NULL;

    if(index >= hs->capacity)
    {
        fprintf(stderr, "index %zu is out of bounds.\n", index);
        return NULL;
    }
    return &hs->slots[index];
}

static bool jet_hs_slot_fill(jet_hs_slot* slot, const void* key, size_t key_size)
{
    if(slot == NULL || key == NULL)
        return false;
    slot->key = malloc(key_size);
    memcpy((char*)slot->key, (const char*)key, key_size);
    return true;
}

static void jet_hs_slot_dispose_all(jet_hs_slot* slot, bool preserve_key)
{
    if(slot == NULL) return;
    if(slot->next != NULL)
        jet_hs_slot_dispose_all(slot->next, preserve_key);
    if(!preserve_key && slot->key)
        free(slot->key);
    free(slot->next);
}

static jet_hs_slot* jet_hs_slot_with_key(jet_hs* hs, const void* key)
{
    if(jet_hs_invalid(hs))
        return NULL;
    if(key == NULL)
    {
        fprintf(stderr, "arg key is null.");
        return NULL;
    }

    jet_hs_slot* slot = jet_hs_slot_at(hs, jet_hs_calculate_index(hs, key));
    while(slot != NULL)
    {
        if(slot->key != NULL && hs->equals(slot->key, key))
            return slot;
        slot = slot->next;
    }
    return NULL;
}

bool jet_hs_init(
        jet_hs* hs,
        size_t capacity,
        uint64_t (*hash) (const void*),
        bool (*equals) (const void*, const void*)
        )
{
    if(hs == NULL) return false;

    if(capacity == 0) 
    {
        capacity = 1;
        fprintf(stderr, "capacity cannot be 0, default set to 1.\n");
    }

    memset(hs, 0, sizeof(*hs));
    hs->capacity = capacity;
    hs->count = 0;
    hs->hash = hash;
    hs->equals = equals;
    hs->slots = (jet_hs_slot*)calloc(capacity, sizeof(jet_hs_slot));
    return hs->slots != NULL;
}

void jet_hs_dispose_full(jet_hs* hs)
{
    if(jet_hs_invalid(hs))
        return;
    jet_hs_slot* temp = hs->slots;
    for(size_t i = 0; i < hs->capacity; i++, temp++)
        jet_hs_slot_dispose_all(temp, false);
    free(hs->slots);
}

void jet_hs_dispose_preserve_keys(jet_hs* hs)
{
    if(jet_hs_invalid(hs)) 
        return;
    jet_hs_slot* temp = hs->slots;
    for(size_t i = 0; i < hs->capacity; i++, temp++)
        jet_hs_slot_dispose_all(temp, true);
    free(hs->slots);
}

bool jet_hs_remove(jet_hs* hs, const void* key)
{
    jet_hs_slot* slot = jet_hs_slot_with_key(hs, key);
    if(slot == NULL) 
        return false; 
    if(slot->key) 
    {
        free(slot->key);
        slot->key = NULL;
        return true;
    }
    return false;
}

bool jet_hs_contains(jet_hs* hs, const void* key, const void** out_key)
{
    jet_hs_slot* slot = jet_hs_slot_with_key(hs, key);
    if(slot != NULL) 
    {
        if(out_key != NULL)
            *out_key = slot->key;
        return true;
    }
    return false;
}

const void* jet_hs_add(jet_hs* hs, const void* key, size_t key_size)
{
    if(jet_hs_invalid(hs))
        return NULL;

    if(key == NULL) 
    {
        fprintf(stderr, "arg key cannot be null.\n");
        return NULL;
    }

    const void* out_key;
    if(jet_hs_contains(hs, key, &out_key))
        return (const void*)out_key;
    
    jet_hs_slot* slot = jet_hs_get_available_slot(hs, jet_hs_calculate_index(hs, key));

    if(slot == NULL)
    {
        fprintf(stderr, "failed to aquire available hashset slot.\n");
        return NULL;
    }

    if(jet_hs_slot_fill(slot, key, key_size) == false)
    {
        fprintf(stderr, "failed to add key to slot.\n");
        return NULL;
    }

    return (const void*)slot->key;
}

#endif






















