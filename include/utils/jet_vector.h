#pragma once

#include <stdbool.h>
#include <stddef.h>

jet_vector_t* jet_vector_create(size_t capacity, size_t elm_size);
bool jet_vector_dispose(jet_vector_t* v);
bool jet_vector_append(jet_vetor_t* v, const void* data);
bool jet_vector_remove(jet_vector_t* v, size_t i);
bool jet_vector_clear(jet_vector_t* v);


































































