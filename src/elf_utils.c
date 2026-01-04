#include <elf_utils.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char* load_text_file(const char* path, size_t* out_len)
{
    FILE* file = fopen(path, "r");
    if(!file)
    {
        fprintf(stderr, "error: could not open file at path %s\n", path);
        return NULL;
    }
    
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    if(file_size < 0)
    {
        fprintf(stderr, "error: failed to determine size of file: %s\n", path);
        fclose(file);
        return NULL;
    }
    
    char* buf = (char*)malloc(file_size + 1);
    if(!buf)
    {
        fprintf(stderr, "error: failed to allocate buffer memory\n"); 
        fclose(file);
        return NULL;
    }

    size_t read_size = fread(buf, 1, file_size, file);
    if(read_size != file_size)
    {
        fprintf(stderr, "error: failed to read to file properly");
        free(buf);
        fclose(file);
        return NULL;
    }

    buf[file_size] = NULL_TERM;
    fclose(file);
    
    if(out_len)
        *out_len = read_size;

    return buf;
}



















