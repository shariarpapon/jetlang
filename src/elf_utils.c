#include <stdlib.h>
#include <elf_utils.h>

_bool elf_isWhiteSpace(char c)
{
    switch(c)
    {
        case ' ': case '\t': case '\n':
        case '\v': case '\r': case '\f':
            return _true;
        
        default:
            return _false;
    }
}












