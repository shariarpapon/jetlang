#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef struct jet_darray jet_darray;

jet_darray* jet_darray_create(size_t capacity, size_t elm_size);
jet_darray* jet_darray_create_copy(jet_darray* darray);

bool jet_darray_dispose(jet_darray* darray);
bool jet_darray_clear(jet_darray* darray);
bool jet_darray_is_empty(jet_darray* darray);
bool jet_darray_insert(jet_darray* darray, size_t i, const void* data);
bool jet_darray_append(jet_darray* darray, const void* data);
bool jet_darray_prepend(jet_darray* darray, const void* data);

bool jet_darray_remove(jet_darray* darray, size_t i);
bool jet_darray_remove_range(jet_darray* darray, size_t start, size_t end);
bool jet_darray_remove_first(jet_darray* darray);
bool jet_darray_remove_last(jet_darray* darray);

void* jet_darray_get(jet_darray* darray, size_t i);
void* jet_darray_get_first(jet_darray* darray);
void* jet_darray_get_last(jet_darray* darray);
size_t jet_darray_count(jet_darray* darray);
































































