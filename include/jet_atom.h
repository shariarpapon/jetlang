#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct jet_atom
{
    size_t start;
    size_t end;
} jet_atom;

bool jet_atom_init(jet_atom* span, size_t start, size_t end);
bool jet_atom_init_copy(jet_atom* target, jet_atom* src);
bool jet_atom_dispose(jet_atom* span);
