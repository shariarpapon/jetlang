#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <elf_lexer.h>
#include <elf_utils.h>

static int arg_count;
static char** args;

void print_source(const char* src, size_t src_len);
BOOL find_arg(const char* target);

int main(int argc, char** argv)
{
    arg_count = argc;
    args = argv;

    size_t src_len = 0;
    const char* source = (const char*)load_text_file("elfcode.ef", &src_len);
    if(!source)
    {
        fprintf(stderr, "error: could not load source text\n");
        return FALSE;
    }
    
    if(find_arg("a") == TRUE || find_arg("s") ==  TRUE)
        print_source(source, src_len);
    
    elf_lexer* lexer = elf_lexer_create(source);
    
    if(!lexer)
        return FALSE;

    if(!lexer->tokens)
        return FALSE; 

    elf_lexer_tokenize(lexer);
   
    if(find_arg("a") == TRUE || find_arg("t"))
        elf_token_print_list((elf_token**)lexer->tokens->elements, lexer->tokens->count); 
   
    elf_lexer_full_dispose(lexer); 
    free((void*)source);

    printf("* all tests ran successfully\n");
    return TRUE;
}

void print_source(const char* source, size_t src_len)
{   
    puts("\n*****************************************************\n");
    printf("source text loaded (len: %zu):\n\n%s", src_len, source);
    puts("\n*****************************************************\n");
}

BOOL find_arg(const char* target)
{
    if(arg_count <= 0 || !target || !args) 
        return FALSE;

    for(int i = 0; i < arg_count; i++)
    {
        if(strcmp(args[i], target) == 0)
            return TRUE;
    }
    return FALSE;
}
