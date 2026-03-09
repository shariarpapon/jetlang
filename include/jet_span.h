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
