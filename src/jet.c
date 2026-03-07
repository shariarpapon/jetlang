#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <jet_io.h>
#include <jet_lexer.h>
#include <jet_ast.h>
#include <jet_ast_print.h>

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

    jet_lexer lexer;
    assert(jet_lexer_init(&lexer, src) && "could not init lexer");
    assert(jet_lexer_tokenize(&lexer) && "could not tokenize");
    jet_token_print_da(&lexer.token_da);


    jet_ast ast;
    assert(jet_ast_init(&ast, (const jet_da*)&lexer.token_da) && "could not init ast");
    
    assert(jet_ast_generate_nodes(&ast) && "node generation failed, ast in corrupted state");
    jet_ast_print((const jet_ast*)&ast);

    jet_lexer_dispose(&lexer);
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

