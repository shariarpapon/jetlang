#pragma once
#include <stdlib.h>
#include <stdbool.h>

typedef struct List
{
    size_t capacity;
    size_t count;
    void** elements;
} vec;


vec* vec_create(size_t capacity);

void vec_increase_capacity(vec* vec_ptr, size_t new_capacity);
void vec_dispose(vec* vec_ptr);

bool vec_append(vec* vec_ptr, void* data);
bool vec_remove(vec* vec_ptr, void* data);
bool vec_remove_at(vec* vec_ptr, size_t index);
bool vec_find(vec* vec_ptr, void* data);

void* vec_get(vec*  vec_ptr, size_t index); 
void* vec_peek(vec* vec_ptr);
void* vec_pop(vec* vec_ptr);








































