#pragma once

#include <stdlib.h>
#include <tbdefines.h>

typedef struct List
{
    size_t capacity;
    size_t count;
    void** elements;
} tblist;


tblist* tblist_create(size_t capacity);

void tblist_increase_capacity(tblist* list, size_t newCapacity);
void tblist_dispose(tblist* list);

_bool tblist_append(tblist* list, void* data);
_bool tblist_remove(tblist* list, void* data);
_bool tblist_remove_at(tblist* list, size_t index);

void* tblist_get(tblist*  list, size_t index); 
void* tblist_find(tblist* list, void* data);
void* tblist_peek(tblist* list);
void* tblist_pop(tblist* list);








































