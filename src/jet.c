#include <jet_lexer.h>
#include <jet_parser.h>
#include <jet_ast.h>
#include <jet_ast_print.h>

#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <jet_io.h>


static int arg_count = 0;
static char** args = NULL;

static const char* jet_get_arg_at(size_t index);

int main(int argc, char** argv)
{
    assert(argc >= 2 && "no filepath provided");
    printf("codebase compiled successfully.\n");

    arg_count = argc;
    args = argv;
    
    const char* filepath = jet_get_arg_at(1);
    size_t src_len = 0;
    const char* src = jet_io_read_text(filepath, &src_len);
    
    assert(src != NULL && "could not read source file");
    if(src_len == 0)
    {
        printf("read file is empty.\n");
        return 0;
    }

    jet_da token_da;
    assert(jet_da_init(&token_da, 32, sizeof(jet_token)) && "could not init token da");

    jet_ast ast;
    assert(jet_ast_init(&ast) && "could not init ast."); 

    jet_lexer lexer;
    assert(jet_lexer_init(&lexer, src, &token_da) && "could not init lexer.");    
    assert(jet_lexer_tokenize(&lexer) && "could not tokenize.");

    jet_parser parser;
    assert(jet_parser_init(&parser, &token_da, &ast) && "could not init parser.");
    assert(jet_parser_parse(&parser) && "could not finish parsing.");

    jet_parser_dispose(&parser);
    jet_ast_dispose(&ast);
    jet_lexer_dispose(&lexer);

    free((void*)src);
    return 0;
}

static const char* jet_get_arg_at(size_t index)
{
    if(index >= arg_count)
    {
        fprintf(stderr, "cannot get arg, index = %zu is out of bounds.\n", index); 
        abort();
        return NULL;
    }
    char** temp = args;
    for(size_t i = 0; i < index; i++)
        temp++;
    return (const char*)*temp;

}

