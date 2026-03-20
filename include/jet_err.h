#pragma once
#include <jet_logger.h>
#include <jet_span.h>
#include <inttypes.h>

#define JET_ERR_LOC_FMT "[line: " PRIu32 ", col: " PRIu32 "]"

#define JET_ERR_UNEXP_TOK(t) \
    JET_ERROR(JET_ERR_LOC_FMT " unexpected token.", (t)->span.line, (t)->span.col)

