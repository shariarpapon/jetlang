#include <utils/jet_list.h>
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

// returns true if list memroy was re-allocated
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
    memset((char*)v->data_array, 0, v->elm_size * v->count);
    v->count = 0;
    return true;
}

void* jet_list_top(jet_list* list)
{
    return jet_list_get(list, list->count - 1);
}

void* jet_list_bottom(jet_list* list)
{
    return jet_list_get(list, 0);
}

bool jet_list_insert(jet_list* list, size_t i, const void* data)
{
    if(!list)
    {
        fprintf(stderr, "error: cannot insert element, invalid arg list.\n");
        return false;
    }
    
    if(i > list->count)
    {   jet_list_shr_n(list, list->count, i - list->count);
        memcpy((char*)list->data_array + list->elm_size * list->count, data, list->elm_size);
    }
    else
    {
        jet_list_shr(list, i);
        memcpy((char*)list->data_array + list->elm_size  * i, data, list->elm_size); 
    }
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

bool jet_list_shl(jet_list* list, size_t start)
{
    return jet_list_shl_n(list, start, 1);
}

bool jet_list_shr(jet_list* list, size_t start)
{
    return jet_list_shr_n(list, start, 1);
}

bool jet_list_shr_n(jet_list* list, size_t start, size_t n)
{
    if(n == 0) return true;
    if(!list)
    {
        fprintf(stderr, "error: cannot shift-right, arg list invalid.\n");
        return false;
    }
    jet_list_ensure_capacity(list, list->count + n);
    memmove(
        //dest
        (char*)list->data_array + list->elm_size * (start + n),
        //src
        (char*)list->data_array + list->elm_size * start, 
        //stride          
        list->elm_size * (list->count - start)
    );
    list->count += n;
    return true;
}

bool jet_list_shl_n(jet_list* list, size_t start, size_t n)
{
    if(n == 0) return true;
    if(!list)
    {
        fprintf(stderr, "error: cannot shift left, arg list invalid.\n");
        return false;
    }    

    memcpy(
        //dest
        (char*)list->data_array + list->elm_size * (start - n),
        //src
        (char*)list->data_array + list->elm_size * (start + 1),
        //stride
        list->elm_size * (list->count - start - 1)
    );
    list->count -= n;
    return true;
}

bool jet_list_remove(jet_list* list, size_t i)
{ 
    return jet_list_shl(list, i);
}

bool jet_list_remove_top(jet_list* list)
{
    if(!list)
    {
        fprintf(stderr, "error: cannot remove top, invalid arg list.\n");
        return false;
    }
    
    if(list->count == 0) return true;
    
    return jet_list_remove(list, list->count - 1); 
}

bool jet_list_remove_bottom(jet_list* list)
{
    if(!list)
    {
        fprintf(stderr, "error: cannot remove bottom, invalid arg list.\n");
        return false;
    }
    
    if(list->count == 0) return true;    
    return jet_list_remove(list, 0); 
}


bool jet_list_pinch(jet_list* list, size_t from, size_t qt, jet_list* out_list)
{
    if(!list)
    {
         fprintf(stderr, "error: cannot pinch jet_list, arg list and/or arg out_list is invalid.\n");
         return false;
    }
   
    if(qt == 0) return true;
    
    if(out_list && list->elm_size != out_list->elm_size)
    {
        fprintf(stderr, "error: cannot pinch jet_list, out_list elm_size mis-match.\n");
        return false;
    }

    if(from >= list->count || from + qt > list->count)
    {
         fprintf(stderr, "wrn: cannot pinch jet_list, arg from and/or qt is greater than list.count.\n");
         return false;
    }
  
    for(size_t i = 0; i < qt; i++) 
    {
        if(out_list)
        {
            void* data = jet_list_get(list, from + i);
            if(!jet_list_append(out_list, (const void*)data))
            {
                fprintf(stderr, "error: cannot pinch jet_list, failed to append to arg out_list.\n");
                return false;
            }
        }

        if(!jet_list_remove(list, from))
        {
            fprintf(stderr, "error: cannot pinch jet_list, failed to remove from arg list.\n");
            return false;
        }
    }
    return true;
}

void* jet_list_get(jet_list* list, size_t i)
{
    if(i >= list->count)
    {
        fprintf(stderr, "error: cannot retrive jet_list element, index out of bounds.\n");
        return NULL;
    }
    return (char*)list->data_array + list->elm_size * i;
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
    if(list->capacity > min_cap)
        return false;
    void* new_array = malloc(min_cap * UPSIZE_FAC * list->elm_size);
    if(!new_array)
    {
        fprintf(stderr, "error: cannot to resize list, memory allocation failed.\n");
        return false;
    }
    memcpy(new_array, list->data_array, list->count * list->elm_size); 
    free(list->data_array);
    list->data_array = new_array;
    return true;       
}






















