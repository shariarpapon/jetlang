#include <jet_conv.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int jet_conv_stoi(const char* start, size_t len)
{
    if(!start)
    {
        fprintf(stderr, "error: cannot convert, start string is invalid.\n");
        return 0;
    }
    char buf[len + 1];
    memcpy(buf, (void*)start, len * sizeof(char));
    buf[len] = '\0';
    int i = atoi(buf);
    return i;
}


float jet_conv_stof(const char* start, size_t len)
{
    if(!start)
    {
        fprintf(stderr, "error: cannot convert, start string is invalid.\n");
        return 0;
    }
    char buf[len + 1];
    memcpy(buf, (void*)start, len * sizeof(char));
    buf[len] = '\0';
    float f = atof(buf);
    return f;
}

