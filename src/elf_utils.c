#include <elf_utils.h>
#include <stdlib.h>
#include <stdio.h>

char* read_text_file(const char* path)
{
    FILE* file = fopen(path, "r");
    if(!file)
    {
        fprintf(stderr, "error: could not open file at path %s\n", path);
        return NULL;
    }
    
    long file_size = ftell(file);
    rewind(file);

    if(file_size < 0)
    {
        fprintf(stderr, "failed to determine size of file: %s\n", path);
        fclose(file);
        return NULL;
    }
    

    fclose(file);
    return "";
}

























