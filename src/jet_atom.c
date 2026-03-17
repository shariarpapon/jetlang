#include <jet_atom.h>

#include <string.h>

bool jet_atom_init(jet_atom* span, size_t start, size_t end)
{
    if(!span)
        return false;
    memset(span, 0, sizeof(*span));
    span->start = start;
    span->end = end;
    return true;
}

bool jet_atom_init_copy(jet_atom* target, jet_atom* src)
{
    if(!target || !src)
        return false;
    return jet_atom_init(target, src->start, src->end);
}

bool jet_atom_dispose(jet_atom* span)
{
    if(!span) 
        return false;
    memset(span, 0, sizeof(*span));
    return true;
}




