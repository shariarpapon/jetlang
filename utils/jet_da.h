#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef struct jet_da
{
    size_t capacity;
    size_t count;
    size_t elm_size;
    void* block;
} jet_da;

jet_da jet_da_default(size_t capacity, size_t elm_size);
bool jet_da_init(jet_da* da, size_t capacity, size_t elm_size);
bool jet_da_clone(jet_da* dest, const jet_da* source);

bool jet_da_dispose(jet_da* da);
bool jet_da_clear(jet_da* da);
bool jet_da_is_empty(const jet_da* da);
bool jet_da_insert(jet_da* da, size_t i, const void* data);
bool jet_da_append(jet_da* da, const void* data);
bool jet_da_prepend(jet_da* da, const void* data);

bool jet_da_remove(jet_da* da, size_t i);
bool jet_da_remove_range(jet_da* da, size_t start, size_t end);
bool jet_da_remove_first(jet_da* da);
bool jet_da_remove_last(jet_da* da);

void* jet_da_get(const jet_da* da, size_t i);
void* jet_da_get_first(const jet_da* da);
void* jet_da_get_last(const jet_da* da);
size_t jet_da_count(const jet_da* da);

#ifdef JET_DA_IMPL

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

#define JET_DA_GROWTH_FAC (2)

static bool jet_da_ensure_capacity(jet_da* da, size_t min_cap);

jet_da jet_da_create(size_t capacity, size_t elm_size)
{
   jet_da da = {0};
   if(!jet_da_init(&da, capacity, elm_size))
        fprintf(stderr, "err: failed to init da block.\n"); 
   return da;
}

bool jet_da_init(jet_da* da, size_t capacity, size_t elm_size)
{
    if(!da) 
    {
        fprintf(stderr, "err: cannot initialize, given da is null.\n");
        return false;
    }

    if(capacity == 0)
    {
        fprintf(stderr, "err: cannot initialize, capacity must be > 0.\n");
        return false;
    }

    if(elm_size == 0)
    {
        fprintf(stderr, "err: cannot initialize, element size must > 0\n");
        return false;
    }

    da->block = malloc(capacity * elm_size);
    if(!da->block)
    {
        fprintf(stderr, "err: failed to allocate array memory.\n");
        return false;
    }
    da->count = 0;
    da->capacity = capacity;
    da->elm_size = elm_size;
    return true;
}

bool jet_da_clone(jet_da* dest, const jet_da* source)
{
    if(dest == NULL || source == NULL)
    {
        fprintf(stderr, "err: failed to copy, invalid arg.\n");
        return false;
    }
    if(dest == source)
        return true;
    
    dest->elm_size = source->elm_size; 
    if(!jet_da_ensure_capacity(dest, source->capacity))
    {
        fprintf(stderr, "err: unable to copy, failed to ensure capacity.\n");
        return false;
    }
    memcpy(dest->block, source->block, source->elm_size * source->count);
    dest->count = source->count;
    return true;
}

bool jet_da_dispose(jet_da* da)
{
    if(da == NULL)
        return false;
    free(da->block);
    da->block = NULL;
    da->capacity = 0;
    da->count = 0;
    da->elm_size = 0;
    return true;
}

bool jet_da_clear(jet_da* v)
{
    if(!v || !v->block)
    {
       return false;
    }
    if(v->count == 0)
        return true;
    memset(v->block, 0, v->elm_size * v->count);
    v->count = 0;
    return true;
}

bool jet_da_is_empty(const jet_da* da)
{
    if(!da)
    {
        fprintf(stderr, "error: invalid arg da.\n");
        return true;
    }
    if(da->count == 0) return true;
    return false;
}

bool jet_da_insert(jet_da* da, size_t i, const void* data)
{
    if(!da)
    {
        fprintf(stderr, "error: cannot insert element, invalid arg da.\n");
        return false;
    }
    
    if(i >= da->count)
    {
        fprintf(stderr, "err: index out of bounds.\n");
        return false;
    }
    
    if(!jet_da_ensure_capacity(da, da->count + 1))
    {
        return false;
    }
    
    if(i != da->count)
    {
        //shift right: map [i, count - 1] --> [i + 1, count]
        memmove(
            (char*)da->block + da->elm_size * (i + 1),
            (char*)da->block + da->elm_size * i,      
            da->elm_size * (da->count - i)            
        );
    }
    memcpy((char*)da->block + da->elm_size  * i, data, da->elm_size); 
    da->count++;
    return true;
}

bool jet_da_prepend(jet_da* da, const void* data)
{
    return jet_da_insert(da, 0, data);
}

bool jet_da_append(jet_da* da, const void* data)
{
    return jet_da_insert(da, da->count, data);
}

bool jet_da_remove(jet_da* da, size_t i)
{ 
    if(!da)
    {
        fprintf(stderr, "error: cannot remove, invalid arg da.\n");
        return false;
    }
    else if(da->count == 0)
    {
        fprintf(stderr, "error: cannot remove, da is empty.\n");
        return false;
    }
    else if(i >= da->count)
    {
        fprintf(stderr, "error: cannot remove, index %llu is out of bounds.\n", (unsigned long long)i);
        return false;
    }
    else if(i == da->count - 1)
    {
        da->count--;
        return true;
    }
    memmove(
        (char*)da->block + da->elm_size * i,//dest
        (char*)da->block + da->elm_size * (i + 1),//src
        da->elm_size * (da->count - (i + 1))//size
    );
    da->count--;
    return true;
}

bool jet_da_remove_range(jet_da* da, size_t start, size_t end)
{ 
    if(!da)
    {
        fprintf(stderr, "error: cannot remove range, invalid arg da.\n");
        return false;
    }
    else if(da->count == 0)
    {
        fprintf(stderr, "error: cannot remove range, da is empty.\n");
        return false;
    }
    else if(end >= da->count || start > end)
    {
        fprintf(stderr, "error: cannot remove range [%zu, %zu], invalid bounds.\n",start, end);
        return false;
    }
    
    size_t len = end - start  + 1;
    if(end == da->count - 1)
    {
        da->count -= len;
        return true;
    }
    memmove(
        (char*)da->block + da->elm_size * start,
        (char*)da->block + da->elm_size * (end + 1),
        da->elm_size * (da->count - (end + 1))
    );
    da->count -= len;
    return true;
}

bool jet_da_remove_first(jet_da* da)
{
    return jet_da_remove(da, 0); 
}

bool jet_da_remove_last(jet_da* da)
{
    return jet_da_remove(da, da->count - 1); 
}

void* jet_da_get(const jet_da* da, size_t i)
{
    if(!da)
    {
        fprintf(stderr, "error: cannot get element, invalid arg da.\n");
        return NULL;
    }

    if(da->count == 0)
    {
        fprintf(stderr, "wrn: cannot get element, da is empty.\n");
    }

    if(i >= da->count)
    {
        fprintf(stderr, "error: cannot get element, index out of bounds.\n");
        return NULL;
    }
    return (char*)da->block + da->elm_size * i;
}

void* jet_da_get_first(const jet_da* da)
{
    return jet_da_get(da, 0);
}

void* jet_da_get_last(const jet_da* da)
{
    return jet_da_get(da, da->count - 1);
}

size_t jet_da_count(const jet_da* da)
{
    if(!da)
    {
        fprintf(stderr, "err: provided jet_da is invalid.\n");
        return 0;
    }
    return da->count;
}

static bool jet_da_ensure_capacity(jet_da* da, size_t min_cap)
{
    assert(da != NULL && "unable to ensure cap, arg da null.");
    if(da->capacity >= min_cap)
        return true;
    size_t new_cap = da->capacity;
    while(new_cap < min_cap)
    {
        if(new_cap > SIZE_MAX / JET_DA_GROWTH_FAC)
            goto fail_cap_overflow;
        new_cap *= JET_DA_GROWTH_FAC;
    }
    
    if(new_cap > SIZE_MAX / da->elm_size)
        goto fail_cap_overflow;

    size_t bytes = new_cap * da->elm_size;
    void* new_block = realloc(da->block, bytes);
    if(!new_block)
    {
        fprintf(stderr, "err: failed to realloc block.\n");
        return false;
    }
    da->block = new_block;
    da->capacity = new_cap;
    return true;  

fail_cap_overflow:
    fprintf(stderr, "err: failed to resize, capacity overflow.\n");
    return false;
}

#endif




























































