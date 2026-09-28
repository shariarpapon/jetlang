#pragma once
#include <stdbool.h>
#include <jet_token.h>

typedef struct jet_error
{
    jet_span span;
    jet_token_type expected;
    jet_token_type found;
} jet_error;

