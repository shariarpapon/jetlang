#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef struct jet_vector jet_vector;

jet_vector* jet_vector_create(size_t capacity, size_t elm_size);
bool jet_vector_dispose(jet_vector* v);
bool jet_vector_append(jet_vector* v, const void* data);
bool jet_vector_remove(jet_vector* v, size_t i);
bool jet_vector_clear(jet_vector* v);
void* jet_vector_get(jet_vector* v, size_t i);
size_t jet_vector_count(jet_vector* v);
































































