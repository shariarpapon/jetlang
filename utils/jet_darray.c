#include <jet_darray.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define UPSIZE_FAC 2

struct jet_darray
{
    size_t capacity;
    size_t count;
    size_t elm_size;
    void* data_array;
};

static bool jet_darray_ensure_capacity(jet_darray* darray, size_t min_cap);

jet_darray* jet_darray_create(size_t capacity, size_t elm_size)
{
    jet_darray* v = (jet_darray*)malloc(sizeof(jet_darray));
    if(!v)
    {
        fprintf(stderr, "cannot create jet_darray, memory allocation failed.\n");
        return NULL;
    }

    if(elm_size == 0)
    {
        fprintf(stderr, "cannot initialize darray with element size of 0 bytes.\n");
        free(v);
        return NULL;
    }

    if(capacity == 0) capacity = 1;

    v->capacity = capacity;
    v->count = 0;
    v->elm_size = elm_size;
    v->data_array = malloc(v->capacity * v->elm_size);
    if(!v->data_array)
    {
        fprintf(stderr, "cannot to resize darray, memory allocation failed.\n");
        free(v);
        return NULL;
    }
    return v;
}

jet_darray* jet_darray_create_copy(jet_darray* darray)
{
    assert(darray != NULL);
    jet_darray* new_darray = jet_darray_create(darray->capacity, darray->elm_size);
    assert(new_darray != NULL);
    memcpy((void*)new_darray->data_array, (const void*)darray->data_array, darray->elm_size * darray->count);
    new_darray->count = darray->count;
    return new_darray;
}

bool jet_darray_dispose(jet_darray* v)
{
    if(!v) return false;
    if(v->data_array)
    {
        free(v->data_array);
        v->data_array = NULL;
    }
    free(v);    
    return true;
}

bool jet_darray_clear(jet_darray* v)
{
    if(!v || !v->data_array)
    {
       return false;
    }
    memset(v->data_array, 0, v->elm_size * v->count);
    v->count = 0;
    return true;
}

bool jet_darray_is_empty(jet_darray* darray)
{
    if(!darray)
    {
        fprintf(stderr, "error: invalid arg darray.\n");
        return true;
    }
    if(darray->count == 0) return true;
    return false;
}

bool jet_darray_insert(jet_darray* darray, size_t i, const void* data)
{
    if(!darray)
    {
        fprintf(stderr, "error: cannot insert element, invalid arg darray.\n");
        return false;
    }
    
    if(i > darray->count)
    {
        fprintf(stderr, "wrn: attempted to insert past darray.count\n");
        i = darray->count;
    }
    
    if(!jet_darray_ensure_capacity(darray, darray->count + 1))
    {
        return false;
    }
    
    if(i != darray->count)
    {
        //shift right: map [i, count - 1] --> [i + 1, count]
        memmove(
            (char*)darray->data_array + darray->elm_size * (i + 1), // dest
            (char*)darray->data_array + darray->elm_size * i,       // src
            darray->elm_size * (darray->count - i)            // size
        );
    }
    memcpy((char*)darray->data_array + darray->elm_size  * i, data, darray->elm_size); 
    darray->count++;
    return true;
}

bool jet_darray_prepend(jet_darray* darray, const void* data)
{
    return jet_darray_insert(darray, 0, data);
}

bool jet_darray_append(jet_darray* darray, const void* data)
{
    return jet_darray_insert(darray, darray->count, data);
}

bool jet_darray_remove(jet_darray* darray, size_t i)
{ 
    if(!darray)
    {
        fprintf(stderr, "error: cannot remove, invalid arg darray.\n");
        return false;
    }
    else if(darray->count == 0)
    {
        fprintf(stderr, "error: cannot remove, darray is empty.\n");
        return false;
    }
    else if(i >= darray->count)
    {
        fprintf(stderr, "error: cannot remove, index %llu is out of bounds.\n", (unsigned long long)i);
        return false;
    }
    else if(i == darray->count - 1)
    {
        darray->count--;
        return true;
    }
    memmove(
        (char*)darray->data_array + darray->elm_size * i,//dest
        (char*)darray->data_array + darray->elm_size * (i + 1),//src
        darray->elm_size * (darray->count - (i + 1))//size
    );
    darray->count--;
    return true;
}

bool jet_darray_remove_range(jet_darray* darray, size_t start, size_t end)
{ 
    if(!darray)
    {
        fprintf(stderr, "error: cannot remove range, invalid arg darray.\n");
        return false;
    }
    else if(darray->count == 0)
    {
        fprintf(stderr, "error: cannot remove range, darray is empty.\n");
        return false;
    }
    else if(end >= darray->count || start > end)
    {
        fprintf(stderr, "error: cannot remove range [%llu, %llu], invalid bounds.\n", (unsigned long long)start, (unsigned long long)end);
        return false;
    }
    
    size_t len = end - start  + 1;
    if(end == darray->count - 1)
    {
        darray->count -= len;
        return true;
    }
    memmove(
        (char*)darray->data_array + darray->elm_size * start,//dest
        (char*)darray->data_array + darray->elm_size * (end + 1),//src
        darray->elm_size * (darray->count - (end + 1))//size
    );
    darray->count -= len;
    return true;
}

bool jet_darray_remove_first(jet_darray* darray)
{
    return jet_darray_remove(darray, 0); 
}

bool jet_darray_remove_last(jet_darray* darray)
{
    return jet_darray_remove(darray, darray->count - 1); 
}

void* jet_darray_get(jet_darray* darray, size_t i)
{
    if(!darray)
    {
        fprintf(stderr, "error: cannot get element, invalid arg darray.\n");
        return NULL;
    }

    if(darray->count == 0)
    {
        fprintf(stderr, "wrn: cannot get element, darray is empty.\n");
    }

    if(i >= darray->count)
    {
        fprintf(stderr, "error: cannot get element, index out of bounds.\n");
        return NULL;
    }
    return (char*)darray->data_array + darray->elm_size * i;
}

void* jet_darray_get_first(jet_darray* darray)
{
    return jet_darray_get(darray, 0);
}

void* jet_darray_get_last(jet_darray* darray)
{
    return jet_darray_get(darray, darray->count - 1);
}

size_t jet_darray_count(jet_darray* v)
{
    if(!v)
    {
        fprintf(stderr, "error: provided jet_darray is invalid.\n");
        return 0;
    }
    return v->count;
}

static bool jet_darray_ensure_capacity(jet_darray* darray, size_t min_cap)
{
    if(darray->capacity >= min_cap)
        return true;
    size_t new_cap = darray->capacity;
    while(new_cap < min_cap)
        new_cap *= UPSIZE_FAC;

    void* new_array = malloc(new_cap * darray->elm_size);
    if(!new_array)
    {
        fprintf(stderr, "error: cannot to resize darray, memory allocation failed.\n");
        return false;
    }
    darray->capacity = new_cap;
    memcpy(new_array, darray->data_array, darray->count * darray->elm_size); 
    free(darray->data_array);
    darray->data_array = new_array;
    return true;       
}






















