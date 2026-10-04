#include <jet_error.h>

static jet_error jet_error_base(jet_error_kind kind, jet_span span);

static jet_error jet_error_base(jet_error_kind kind, jet_span span)
{
    jet_error error;
    memset(&error, 0, sizeof(error));
    error.kind = kind;
    error.span = span;
    return error;
}

