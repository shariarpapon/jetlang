#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <jet_lexer.h>
#include <jet_io.h>
#include <jet_list.h>

#define ARG_PRINT_TOKENS "pt"
#define ARG_PRINT_SOURCE "ps"
#define ARG_PRINT_ALL "pa"

static int arg_count;
static char** args;

void print_source(const char* src, size_t src_len);
bool find_arg(const char* target);

int main(int argc, char** argv)
{
    arg_count = argc;
    args = argv;
    
    //skip calling file cmd
    args++;
    arg_count--;

    if(!args)
    {
        fprintf(stderr, "error: no valid source file path provided\n");
        return 1;
    }
    
    size_t src_len = 0;
    //now *arg should point to the source file path (the 2nd arg)
    const char* source = (const char*)jet_io_read_text(*args, &src_len);
    if(!source)
    {
        fprintf(stderr, "error: could not load source text\n");
        return 1;
    }
    
    args++;
    arg_count--;
    
    if(find_arg(ARG_PRINT_ALL) == true || find_arg(ARG_PRINT_SOURCE) ==  true)
        print_source(source, src_len);
    
    jet_lexer* lexer = jet_lexer_create(source);
    
    if(!lexer)
    {
        printf("could not create lexer, exiting...\n");
        return 1;
    } 

    if(!jet_lexer_tokenize(lexer))
    {
        printf("could not tokenize, exiting...\n");
        return 1;
    }
     
    if(find_arg(ARG_PRINT_ALL) == true || find_arg(ARG_PRINT_TOKENS) == true)
        jet_token_print_list(lexer->token_list); 
    
    jet_lexer_dispose(lexer); 
    free((void*)source);

    printf("all tests ran successfully!\n");
    return 0;
}

void print_source(const char* source, size_t src_len)
{   
    puts("\n*****************************************************\n");
    printf("source text loaded (len: %llu):\n\n%s", (unsigned long long)src_len, source);
    puts("\n*****************************************************\n");
}

bool find_arg(const char* target)
{
    if(arg_count <= 0 || !target || !args) 
        return false;

    for(int i = 0; i < arg_count; i++)
    {
        if(strcmp(args[i], target) == 0)
            return true;
    }
    return false;
}
