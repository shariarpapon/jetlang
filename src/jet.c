#include <jet_compilation_unit.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <jet_io.h>
#include <jet_logger.h>
#include <jet_ast_print.h>
#include <jet_token_print.h>

#define JET_FLAG_PRINT_TOKENS ("p-tok")
#define JET_FLAG_PRINT_AST ("p-ast")

static int arg_count = 0;
static char** args = NULL;

static const char* jet_get_arg_at(size_t index);
static const char* jet_get_filepath();
static void jet_init_args(int argc, char** argv);
static bool jet_compile(const char* filepath, jet_compilation_unit* cu);
static bool jet_has_flag(const char* flag);

int main(int argc, char** argv)
{   
    printf("\n");
    jet_init_args(argc, argv);
    
    const char* filepath = jet_get_filepath();
    JET_ASSERT(filepath != NULL);

    JET_LOG_INFO("all internal modules built, initiating jet compiler...");

    jet_compilation_unit cu;
    if(jet_compile(filepath, &cu))
        JET_LOG_INFO("input compiled successfully.");
    else 
        JET_LOG_ERROR("failed to compile input.");
    
    printf("\n");

    if(jet_has_flag(JET_FLAG_PRINT_TOKENS) == true)
        jet_token_tprint_da((const jet_da*)&cu.tok_da);
    if(jet_has_flag(JET_FLAG_PRINT_AST) == true)
        jet_ast_print((const jet_ast*)&cu.ast);

    jet_cu_dispose(&cu);
}

static bool jet_compile(const char* filepath, jet_compilation_unit* cu)
{
    if(!cu) return false;
    if(!jet_cu_init(cu, filepath)) 
        return false;
    return jet_cu_run(cu);
}

static void jet_init_args(int argc, char** argv)
{
    JET_ASSERT(argv != NULL);
    JET_ASSERT(*argv != NULL);
    arg_count = argc;
    args = argv; 
}

static const char* jet_get_filepath()
{ 
    const char* filepath = jet_get_arg_at(1);
    JET_ASSERTM(filepath != NULL, "no valid filepath arg.");
    return filepath;
}

static const char* jet_get_arg_at(size_t index)
{
    if(index >= arg_count)
    {
        JET_LOG_FATAL("cannot get arg, index = %zu is out of bounds.", index); 
    }
    char** temp = args;
    for(size_t i = 0; i < index; i++)
        temp++;
    return (const char*)*temp;

}

static bool jet_has_flag(const char* flag)
{
    if(arg_count < 3) return false;
    for(size_t i = 2; i < arg_count; i++)
        if(strcmp(jet_get_arg_at(i), flag) == 0)
            return true;
    return false;
}

