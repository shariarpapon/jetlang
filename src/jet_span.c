#include <jet_span.h>

#include <string.h>

bool jet_span_init(jet_span* span, size_t start, size_t end, uint32_t line, uint32_t col)
{
    if(!span)
        return false;
    memset(span, 0, sizeof(*span));
    span->start = start;
    span->end = end;
    span->line = line;
    span->col = col;
    return true;
}

bool jet_span_init_copy(jet_span* target, jet_span* src)
{
    if(!target || !src)
        return false;
    return jet_span_init(target, src->start, src->end, src->line, src->col);
}

bool jet_span_dispose(jet_span* span)
{
    if(!span) 
        return false;
    memset(span, 0, sizeof(*span));
    return true;
}




