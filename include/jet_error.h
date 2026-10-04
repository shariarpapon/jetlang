#pragma once
#include <stdbool.h>
#include <jet_span.h>

typedef enum jet_error_kind
{
    JET_ERR_UNDEFINED,

} jet_error_kind;

typedef struct jet_error
{
    jet_error_kind kind;
    jet_span span;
    
    union
    {
        
    } as;
} jet_error;






