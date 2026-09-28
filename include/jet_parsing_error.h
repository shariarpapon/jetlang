#pragma once
#include <stdbool.h>
#include <jet_token.h>

typedef enum jet_parsing_error_kind
{
    JET_PERROR_UNKNOWN,
    JET_PERROR_UNEXP_TOKEN,
    JET_PERROR_EXP_TOKEN,
    JET_PERROR_INVALID_SYNTAX,
    JET_PERROR_UNEXP_EOF
} jet_parsing_error_kind;

typedef struct jet_parsing_error
{
    jet_parsing_error_kind kind;
    jet_span span;
    jet_token_type expected;
    jet_token_type found;
} jet_parsing_error;

