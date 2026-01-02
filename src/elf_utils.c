#include <elf_utils.h>

BOOL* elf_newBool()
{
    BOOL* ptr = (BOOL*)malloc(sizeof(BOOL));
    if(!ptr)
    {
        ptr = NULL;
        fprintf(stderr, "error: unable to allocate new BOOL memory.");
        return (BOOL*){0};
    }
    *ptr = FALSE;
    return ptr;
}

void elf_disposeBool(BOOL* b)
{
    if(b)
        free(b);
}

































