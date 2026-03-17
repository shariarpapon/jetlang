#include <jet_atom.h>

#include <string.h>

bool jet_atom_init(jet_atom* span, size_t start, size_t end, uint32_t line, uint32_t col)
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

bool jet_atom_init_copy(jet_atom* target, jet_atom* src)
{
    if(!target || !src)
        return false;
    return jet_atom_init(target, src->start, src->end, src->line, src->col);
}

bool jet_atom_dispose(jet_atom* span)
{
    if(!span) 
        return false;
    memset(span, 0, sizeof(*span));
    return true;
}




