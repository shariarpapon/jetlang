#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <assert.h>
#include <jet_lexer.h>
#include <jet_io.h>
#include <jet_list.h>
#include <jet_ast.h>

#define ARG_PRINT_ALL "pAll"
#define ARG_PRINT_TOKENS "pTok"
#define ARG_PRINT_SOURCE "pSrc"
#define ARG_PRINT_AST "pAst"

static int arg_count;
static char** args;

static void print_source(const char* src, size_t src_len);
static bool find_arg(const char* target);

static jet_lexer* lexer_analyze_file(const char* filename, char** loaded_src_ptr);
static jet_ast* ast_generate(jet_list* tok_list);

int main(int argc, char** argv)
{
    // ------ setup argument ptr ------
   
    arg_count = argc;
    args = argv;
    args++;
    arg_count--;

    if(!args)
    {
        fprintf(stderr, "error: no valid source file path provided\n");
        return 1;
    }

    // -----  create components -----
    
    char* src_filename = *args;
    char** loaded_src_ptr = (char**)malloc(sizeof(char*));

    if(loaded_src_ptr == NULL)
    {
        fprintf(stderr, "error: could not allocate src load buffer.\n");
        return 1;
    }


    jet_lexer* lexer = lexer_analyze_file(src_filename, loaded_src_ptr);
    if(!lexer)
    {
        fprintf(stderr, "error: could not analyze source file.\n");
        return 1;
    }
       
    jet_ast* ast = ast_generate(lexer->token_list); 
    if(!ast)
    {
        fprintf(stderr, "error: could not generate ast.\n");
        return 1;
    }

    // ------ free all memory ------

    if(loaded_src_ptr)
    {
        if(*loaded_src_ptr != NULL)
            free((void*)*loaded_src_ptr);
        free((void*)loaded_src_ptr);
    }


    jet_lexer_dispose(lexer);
    jet_ast_dispose(ast);
    return 0;
}

static jet_lexer* lexer_analyze_file(const char* filename, char** loaded_src_ptr)
{
    if(!filename)
    {
        fprintf(stderr, "error: no valid source filename provided.\n");
        return NULL;
    }

    size_t src_len = -1;
    const char* source = (const char*)jet_io_read_text(filename, &src_len);

    if(!source)
    {
        fprintf(stderr, "error: could not load source text\n");
        return NULL;
    }
    
    if(loaded_src_ptr) *loaded_src_ptr = (char*)source;
    else fprintf(stderr, "wrn: no ptr provided to recieve the loaded source file text (which must be freed manually before disposing lexer).\n");
     
    if(find_arg(ARG_PRINT_ALL) == true || find_arg(ARG_PRINT_SOURCE) ==  true)
        print_source(source, src_len);
    
    jet_lexer* lexer = jet_lexer_create(source);

    if(!lexer)
    {
        fprintf(stderr, "error: could not create lexer.\n");
        return NULL;
    } 

    if(!jet_lexer_tokenize(lexer))
    {
        fprintf(stderr, "error: could not tokenize.\n");
        return lexer;
    }
     
    if(find_arg(ARG_PRINT_ALL) == true || find_arg(ARG_PRINT_TOKENS) == true)
        jet_token_print_list(lexer->token_list);  
    
    return lexer;
}

static jet_ast* ast_generate(jet_list* tok_list)
{
   jet_ast* ast = jet_ast_create(tok_list);
   assert(ast != NULL);
   if(!jet_ast_generate(ast))
   {
        fprintf(stderr, "error: could not generate AST.");
   }
   return ast;
}

static void print_source(const char* source, size_t src_len)
{   
    puts("\n*****************************************************\n");
    printf("source text loaded (len: %llu):\n\n%s", (unsigned long long)src_len, source);
    puts("\n*****************************************************\n");
}

static bool find_arg(const char* target)
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
