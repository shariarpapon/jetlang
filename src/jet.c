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

static jet_lexer* lexer_analyze_file(const char* filename);
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
    char* src_file_path = *args;
    jet_lexer* lexer = lexer_analyze_file(src_file_path); 
    jet_ast* ast = ast_generate(lexer->token_list); 

    // ------ free all memory ------
    
    jet_lexer_dispose(lexer);
    jet_ast_dispose(ast);
    return 0;
}

static jet_lexer* lexer_analyze_file(const char* file_path)
{
    jet_lexer* lexer = jet_lexer_create(file_path);        
    
    if(find_arg(ARG_PRINT_ALL) == true || find_arg(ARG_PRINT_SOURCE) ==  true)
        print_source(lexer->source, lexer->len);

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
   if(!jet_ast_generate_nodes(ast))
        fprintf(stderr, "error: could not generate AST_nodes.");
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
