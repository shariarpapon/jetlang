#include <jet_compilation_unit.h>
#include <stdio.h>
#include <stdbool.h>
#include <jet_io.h>
#include <jet_logger.h>

static int arg_count = 0;
static char** args = NULL;

static const char* jet_get_arg_at(size_t index);
static const char* jet_get_filepath();
static void jet_init_args(int argc, char** argv);
static bool jet_compile(const char* filepath);

int main(int argc, char** argv)
{   
    printf("\n");
    JET_LOG_INFO("all modules built successfully!");
    jet_init_args(argc, argv);
    
    const char* filepath = jet_get_filepath();
    JET_ASSERT(filepath != NULL);

    if(jet_compile(filepath))
        JET_LOG_INFO("input compiled successfully.");
    else 
        JET_LOG_ERROR("failed to compile input.");
    
    printf("\n");
}

static bool jet_compile(const char* filepath)
{
    jet_compilation_unit cu;
    if(!jet_cu_init(&cu, filepath)) 
        return false;
    return jet_cu_run(&cu);
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

