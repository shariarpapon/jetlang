#include <jet_span.h>

#include <string.h>

bool jet_span_init(jet_span* span, size_t len, uint32_t line, uint32_t col)
{
    if(!span)
        return false;
    memset(span, 0, sizeof(*span));
    span->len = len;
    span->line = line;
    span->col = col;
    return true;
}

bool jet_span_init_copy(jet_span* target, jet_span* src)
{
    if(!target || !src) return false;
    if(target == src) return true;
    return jet_span_init(target, src->len, src->line, src->col);
}




