#include <jet_list.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define UPSIZE_FAC 2

struct jet_list
{
    size_t capacity;
    size_t count;
    size_t elm_size;
    void* data_array;
};

static bool jet_list_ensure_capacity(jet_list* list, size_t min_cap);

jet_list* jet_list_create(size_t capacity, size_t elm_size)
{
    jet_list* v = (jet_list*)malloc(sizeof(jet_list));
    if(!v)
    {
        fprintf(stderr, "cannot create jet_list, memory allocation failed.\n");
        return NULL;
    }

    if(elm_size == 0)
    {
        fprintf(stderr, "cannot initialize list with element size of 0\n");
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
        fprintf(stderr, "cannot to resize list, memory allocation failed.\n");
        free(v);
        return NULL;
    }
    return v;
}

jet_list* jet_list_create_copy(jet_list* list)
{
    assert(list != NULL);
    jet_list* new_list = jet_list_create(list->capacity, list->elm_size);
    assert(new_list != NULL);
    memcpy((void*)new_list->data_array, (const void*)list->data_array, list->elm_size * list->count);
    return new_list;
}

bool jet_list_dispose(jet_list* v)
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
bool jet_list_clear(jet_list* v)
{
    if(!v || !v->data_array)
    {
       return false;
    }
    memset(v->data_array, 0, v->elm_size * v->count);
    v->count = 0;
    return true;
}

bool jet_list_is_empty(jet_list* list)
{
    if(!list)
    {
        fprintf(stderr, "error: invalid arg list.\n");
        return true;
    }
    if(list->count == 0) return true;
    return false;
}

bool jet_list_insert(jet_list* list, size_t i, const void* data)
{
    if(!list)
    {
        fprintf(stderr, "error: cannot insert element, invalid arg list.\n");
        return false;
    }
    
    if(i > list->count)
    {
        fprintf(stderr, "wrn: attempted to insert past list.count\n");
        i = list->count;
    }
    
    if(!jet_list_ensure_capacity(list, list->count + 1))
    {
        return false;
    }
    
    if(i != list->count)
    {
        //shift right: map [i, count - 1] --> [i + 1, count]
        memmove(
            (char*)list->data_array + list->elm_size * (i + 1), // dest
            (char*)list->data_array + list->elm_size * i,       // src
            list->elm_size * (list->count - i)            // size
        );
    }
    memcpy((char*)list->data_array + list->elm_size  * i, data, list->elm_size); 
    list->count++;
    return true;
}

bool jet_list_prepend(jet_list* list, const void* data)
{
    return jet_list_insert(list, 0, data);
}

bool jet_list_append(jet_list* list, const void* data)
{
    return jet_list_insert(list, list->count, data);
}

bool jet_list_remove(jet_list* list, size_t i)
{ 
    if(!list)
    {
        fprintf(stderr, "error: cannot remove, invalid arg list.\n");
        return false;
    }
    else if(list->count == 0)
    {
        fprintf(stderr, "error: cannot remove, list is empty.\n");
        return false;
    }
    else if(i >= list->count)
    {
        fprintf(stderr, "error: cannot remove, index %llu is out of bounds.\n", (unsigned long long)i);
        return false;
    }
    else if(i == list->count - 1)
    {
        list->count--;
        return true;
    }
    memmove(
        (char*)list->data_array + list->elm_size * i,//dest
        (char*)list->data_array + list->elm_size * (i + 1),//src
        list->elm_size * (list->count - (i + 1))//size
    );
    list->count--;
    return true;
}

bool jet_list_remove_range(jet_list* list, size_t start, size_t end)
{ 
    if(!list)
    {
        fprintf(stderr, "error: cannot remove range, invalid arg list.\n");
        return false;
    }
    else if(list->count == 0)
    {
        fprintf(stderr, "error: cannot remove range, list is empty.\n");
        return false;
    }
    else if(end >= list->count || start > end)
    {
        fprintf(stderr, "error: cannot remove range [%llu, %llu], invalid bounds.\n", (unsigned long long)start, (unsigned long long)end);
        return false;
    }
    
    size_t len = end - start  + 1;
    if(end == list->count - 1)
    {
        list->count -= len;
        return true;
    }
    memmove(
        (char*)list->data_array + list->elm_size * start,//dest
        (char*)list->data_array + list->elm_size * (end + 1),//src
        list->elm_size * (list->count - (end + 1))//size
    );
    list->count -= len;
    return true;
}

bool jet_list_remove_first(jet_list* list)
{
    return jet_list_remove(list, 0); 
}

bool jet_list_remove_last(jet_list* list)
{
    return jet_list_remove(list, list->count - 1); 
}

bool jet_list_pinch(jet_list* list, size_t from, size_t qt, jet_list* out_list)
{
    return false;
}

void* jet_list_get(jet_list* list, size_t i)
{
    if(!list)
    {
        fprintf(stderr, "error: cannot get element, invalid arg list.\n");
        return NULL;
    }

    if(list->count == 0)
    {
        fprintf(stderr, "wrn: cannot get element, list is empty.\n");
    }

    if(i >= list->count)
    {
        fprintf(stderr, "error: cannot get element, index out of bounds.\n");
        return NULL;
    }
    return (char*)list->data_array + list->elm_size * i;
}

void* jet_list_first(jet_list* list)
{
    return jet_list_get(list, 0);
}

void* jet_list_last(jet_list* list)
{
    return jet_list_get(list, list->count - 1);
}

size_t jet_list_count(jet_list* v)
{
    if(!v)
    {
        fprintf(stderr, "error: provided jet_list is invalid.\n");
        return 0;
    }
    return v->count;
}

static bool jet_list_ensure_capacity(jet_list* list, size_t min_cap)
{
    if(list->capacity >= min_cap)
        return true;
    size_t new_cap = list->capacity;
    while(new_cap < min_cap)
        new_cap *= UPSIZE_FAC;

    void* new_array = malloc(new_cap * list->elm_size);
    if(!new_array)
    {
        fprintf(stderr, "error: cannot to resize list, memory allocation failed.\n");
        return false;
    }
    list->capacity = new_cap;
    memcpy(new_array, list->data_array, list->count * list->elm_size); 
    free(list->data_array);
    list->data_array = new_array;
    return true;       
}






















