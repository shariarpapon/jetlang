#include <utils/jet_vector.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define UPSIZE_FAC 2

struct jet_vector_t
{
    size_t capacity;
    size_t count;
    size_t elm_size;
    void* data_array;
};

void jet_vector_grow(jet_vector_t* v);

jet_vector_t* jet_vector_create(size_t capacity, size_t elm_size)
{
    jet_vector_t* v = (jet_vector_t*)malloc(sizeof(jet_vector_t));
    if(!v)
    {
        perror("cannot create jet_vector, memory allocation failed.\n");
        return NULL;
    }

    if(elm_size == 0)
    {
        perror("cannot initialize vector with element size of 0\n");
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
        perror("cannot to resize vector, memory allocation failed.\n");
        free(v);
        return NULL;
    }
    return v;
}

bool jet_vector_dispose(jet_vector_t* v)
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

bool jet_vector_append(jet_vector_t* v, const void* data)
{
    if(!v) 
    {
        perror("invalid jet_vector provided.\n"); 
        return false;
    }

    if(v->count >= v->capacity)
        jet_vector_grow(v);

    memcpy((char*)v->data_array + v->elm_size * v->count, data, v->elm_size);
    v->count++;
    return true;
}

bool jet_vector_remove(jet_vector_t* v, size_t i)
{
    if(i >= v->count)
    {
        perror("cannot remove element from jet_vector, index out of bounds.\n");
        return false;
    }
    if(i < v->count - 1)
    {
        char* dest = (char*)v->data_array + v->elm_size * i;
        char* from = (char*)v->data_array + v->elm_size * (i + 1); 
        size_t size = v->elm_size * (v->count - i - 1); 
        memcpy(dest, from, size);
    }
    else
    {
        memset((char*)v->data_array + v->count * v->elm_size, 0, v->elm_size);
    }
    v->count--;
    return true;
}

bool jet_vector_clear(jet_vector_t* v)
{
    if(!v || !v->data_array)
    {
       return false;
    }
    memset((char*)v->data_array, 0, v->elm_size * v->count);
    v->count = 0;
    return true;
}

void* jet_vector_get(jet_vector_t* v, size_t i)
{
    if(i >= v->count)
    {
        perror("Cannot retrive jet_vector element, index out of bounds.\n");
        return NULL;
    }
    return (char*)v->data_array + v->elm_size * i;
}

size_t jet_vector_count(jet_vector_t* v)
{
    if(!v)
    {
        perror("Provided jet_vector is invalid.");
        return 0;
    }
    return v->count;
}

void jet_vector_grow(jet_vector_t* v)
{
    v->capacity *= UPSIZE_FAC;
    void* new_array = malloc(v->capacity * v->elm_size);
    if(!new_array)
    {
        perror("Cannot to resize vector, memory allocation failed.\n");
        return;
    }
    memcpy(new_array, v->data_array, v->count * v->elm_size); 
    free(v->data_array);
    v->data_array = new_array;
}

























