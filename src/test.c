#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <elf_lexer.h>
#include <elf_utils.h>

#define ARG_PRINT_TOKENS "pt"
#define ARG_PRINT_SOURCE "ps"
#define ARG_PRINT_ALL "pa"
#define ARG_LEXER_EMIT_TOKENS "lec"

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
    const char* source = (const char*)load_text_file(*args, &src_len);
    if(!source)
    {
        fprintf(stderr, "error: could not load source text\n");
        return 1;
    }
    
    //skip the file path arg
    args++;
    arg_count--;

    
    if(find_arg(ARG_PRINT_ALL) == true || find_arg(ARG_PRINT_SOURCE) ==  true)
        print_source(source, src_len);
    
    elf_lexer* lexer = elf_lexer_create(source);
    
    if(!lexer)
        return 1;

    if(!lexer->tokens)
        return 1; 
    
    if(find_arg(ARG_LEXER_EMIT_TOKENS) == true)
    {
        lexer->emit_comments = true;
    }

    elf_lexer_tokenize(lexer);
   
    if(find_arg(ARG_PRINT_ALL) == true || find_arg(ARG_PRINT_TOKENS))
        elf_token_print_vect((elf_token**)lexer->tokens->elements, lexer->tokens->count); 
    
    elf_lexer_full_dispose(lexer); 
    free((void*)source);

    printf("* all tests ran successfully\n");
    return 0;
}

void print_source(const char* source, size_t src_len)
{   
    puts("\n*****************************************************\n");
    printf("source text loaded (len: %zu):\n\n%s", src_len, source);
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
