#pragma once
#include <stdbool.h>
#include <jet_span.h>

typedef enum jet_error_kind jet_error_kind;

//errors
typedef struct jet_error jet_error;
typedef struct jet_error_unexpected_token jet_error_unexpected_token;
typedef struct jet_error_expected_token jet_error_expected_token;

enum jet_error_kind
{
    JET_ERR_UNKNOWN,
    JET_ERR_UNEXPECTED_EOF,
    JET_ERR_UNEXPECTED_TOKEN,
    JET_ERR_EXPECTED_TOKEN,
    JET_ERR_INVALID_TOKEN,
};

struct jet_error_unexpected_token
{
    jet_token_type found;
};

struct jet_error_expected_token
{
    jet_token_type found;
    jet_token_type expected;
};

struct jet_error
{
    jet_error_kind kind;
    jet_span span;
    union
    {
        jet_error_unexpected_token unexpected_token;
        jet_error_expected_token expected_token;
    } as;
};

jet_error jet_err(jet_error_kind kind, jet_span span);
jet_error jet_err_unexpected_token(const jet_token* tok);
jet_error jet_err_expected_token(const jet_token* found_tok, jet_token_type expected_type);




