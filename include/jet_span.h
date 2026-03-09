#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct jet_span
{
    size_t len;
    uint32_t line;
    uint32_t col;
} jet_span;

bool jet_span_init(jet_span* span, size_t len, uint32_t line, uint32_t col);
bool jet_span_init_copy(jet_span* target, jet_span* src);
