#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#include <toolbox/tblist.h>
#include <toolbox/tbdefines.h>

tblist* tblist_create(size_t capacity)
{
    tblist* list = malloc(sizeof(tblist));
    if(!list)
    {
       list = NULL;
       fprintf(stderr, "error: could not allocate list memory.\n");
       return (tblist*){0};
 
    }

    list->capacity = capacity;
    list->count = 0;
    list->elements = malloc(sizeof(void*) * capacity);
    
    if(!list->elements)
    {
       list->elements = NULL;
       fprintf(stderr, "error: could not allocate list.elements memory.\n");
       return (tblist*){0};
    }

    return list;
}


void tblist_increase_capacity(tblist* list, size_t newCapacity)
{
   if(list->capacity >= newCapacity)
   {
        fprintf(stderr, "warning: cannot increase capacity, new list capacity is less than or equal to current capacity (new-capacity: %zu)\n", newCapacity);
        return;
   }

   void** upsize = malloc(sizeof(void*) * newCapacity);
   if(list->elements)
   {
     for(size_t i = 0; i < list->count; i++)
        upsize[i] = list->elements[i];
   }
   
   free(list->elements);
   list->elements = upsize;
}

void tblist_dispose(tblist* list)
{
    if(!list) return;
    
    if(list->elements)
        free(list->elements);
    free(list);    
} 

void* tblist_pop(tblist* list)
{
    if(!list || !list->elements || 
       list->count <= 0) 
    return NULL;

    void* top = list->elements[list->count - 1];
    tblist_remove_at(list, list->count - 1);
    
    return top;
}

void* tblist_find(tblist* list, void* data)
{
    if(!list) return NULL;
    if(!list->elements) return NULL;
    
    for(int i = 0; i < list->count; i++)
        if(list->elements[i] == data)
            return list->elements[i];
    return NULL;
}

_bool tblist_append(tblist* list, void* data)
{
    if(!list || !list->elements)
    {
        fprintf(stderr, "error: no valid list provided\n");
        return _false;
    }

    if(list->count >= list->capacity)
        tblist_increase_capacity(list, list->capacity * 2);

    list->elements[list->count] = data;
    list->count++;
    return _true;
}


_bool tblist_remove(tblist* list, void* data)
{ 
    if(!data)
    {
        fprintf(stderr, "error: cannot remove, provided element ptr is not valid\n");
        return _false;
    }
    if(!list || !list->elements)
    {
        fprintf(stderr, "error: no valid list provided\n");
        return _false;
    }

    for(size_t i = 0; i < list->count; i++)
        if(list->elements[i] == data)
            return tblist_remove_at(list, i);

    return _false;
}

_bool tblist_remove_at(tblist* list, size_t index)
{
    if(!list || !list->elements)
    {
        fprintf(stderr, "error: no valid list provided\n");
        return _false;
    }
    if(index >= list->count)
    {
        fprintf(stderr, "error: list-element index out of range (index: %zu)\n", index);
        return _false;
    }

    list->elements[index] = NULL;
    for(size_t i = index + 1; i < list->count; i++)
        list->elements[i - 1] = list->elements[i];
    return _true;
}

void* tblist_get(tblist* list, size_t index)
{
    if(!list || !list->elements)
    {
        fprintf(stderr, "error: no valid list provided\n");
        return NULL;
    }
    if(index >= list->count)
    {
        fprintf(stderr, "error: list-element index out of range (index: %zu)\n", index);
        return NULL;
    }
    return list->elements[index];
}






































