#pragma once

#include <stddef.h>
#define NULL_TERM '\0'
#define ARRAY_LEN(a) (sizeof(a) / sizeof(a[0]))
char* jet_io_read_text(const char* file_path, size_t* out_len);
void jet_io_print_str_range(const char* start, size_t len);


#ifdef JET_IO_IMPL

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char* jet_io_read_text(const char* path, size_t* out_len)
{
    FILE* file = fopen(path, "rb");
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

void jet_io_print_str_range(const char* start, size_t len)
{
    if(!start)
    {
        fprintf(stderr, "error: cannot print, given start char* is null.\n");
        return;
    }

    if(len == 0) 
        return;

    for(size_t i = 0; i < len; i++)
    {
        printf("%c", *start);
        start++;
    }
    printf("\n");
}

#endif











