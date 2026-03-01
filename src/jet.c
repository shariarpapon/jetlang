#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <assert.h>
#include <jet_lexer.h>
#include <jet_io.h>
#include <jet_da.h>
#include <jet_ast.h>

#define ARG_PRINT_ALL "pAll"
#define ARG_PRINT_TOKENS "pTok"
#define ARG_PRINT_SOURCE "pSrc"
#define ARG_PRINT_AST "pAst"

static int arg_count;
static char** args;

static void print_source(const char* src, size_t src_len);
static bool find_arg(const char* target);

static jet_lexer* lexer_analyze_file(const char* filename);
static jet_ast* ast_generate(jet_da* tok_da);

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
    char* src_file_path = *args;
    jet_lexer* lexer = lexer_analyze_file(src_file_path); 
    ast_generate(lexer->token_darray); 

    // ------ free all memory ------ 
    jet_lexer_dispose(lexer);
    
    //TEST: jet_ast_dispose(ast);
    printf("* TODO: implement proper node disposal system.\n"); 
    return 0;
}

static jet_lexer* lexer_analyze_file(const char* filepath)
{
    jet_lexer* lexer = jet_lexer_create(); 
    assert(lexer != NULL);

    if(!jet_lexer_init(lexer, filepath))
    {
        fprintf(stderr, "err: could not init lexer.\n");
        return NULL;
    }
    
    if(find_arg(ARG_PRINT_ALL) == true || find_arg(ARG_PRINT_SOURCE) ==  true)
        print_source(lexer->source, lexer->len);

    if(!jet_lexer_tokenize(lexer))
    {
        fprintf(stderr, "error: could not tokenize.\n");
        return lexer;
    }
     
    if(find_arg(ARG_PRINT_ALL) == true || find_arg(ARG_PRINT_TOKENS) == true)
        jet_token_print_darray(lexer->token_darray);  
  
    return lexer;
}


static jet_ast* ast_generate(jet_da* tok_da)
{
   jet_ast* ast = jet_ast_create(tok_da);
   if(!jet_ast_generate_nodes(ast))
        fprintf(stderr, "error: could not generate AST nodes.\n");
   
   if(find_arg(ARG_PRINT_ALL) == true || find_arg(ARG_PRINT_AST) == true)
       jet_ast_print(ast); 
   
   return ast;
}

static void print_source(const char* source, size_t src_len)
{   
    puts("\n*****************************************************\n");
    printf("source text loaded (len: %zu):\n\n%s", src_len, source);
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
