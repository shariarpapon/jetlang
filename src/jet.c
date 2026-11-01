#include <jet_compilation_unit.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <jet_io.h>
#include <jet_logger.h>
#include <jet_ast_print.h>
#include <jet_token_print.h>
#include <jet_cli_flags.h>

#define JET_FILEPATH_SIZE (512)

//PROGRAM
#define JET_FLAG_FILE ("f")

//DEBUG
#define JET_FLAG_PRINT ("p")
#define JET_FLAG_TOKENS ("t")
#define JET_FLAG_AST ("a")
#define JET_FLAG_PARSER_ERRORS ("pe")

static const char* jet_get_filepath();
static bool jet_compile(const char* filepath, jet_compilation_unit* cu);

int main(int argc, char** argv)
{
    argv++;
    argc--;

    jet_flag_registry fr;
    JET_ASSERT(jet_flagreg_init(&fr, argv, argc));

    const jet_flag_arg* file_arg = jet_flagreg_get_argi(&fr, JET_FLAG_FILE, 0);
    if(file_arg == NULL)
    {
        JET_LOG_FATAL("a valid file must be provided as argument (e.g. -f<filepath>)");
        return 1;
    }
    
    char filepath[JET_FILEPATH_SIZE];
    strncpy(filepath, file_arg->name.str, file_arg->name.len);
    JET_ASSERT(filepath != NULL);

    printf("\n");
    JET_LOG_INFO("all internal modules built, initiating jet compiler...");
    jet_compilation_unit cu;
    if(jet_compile(filepath, &cu))
        JET_LOG_INFO("input compiled successfully.");
    else 
        JET_LOG_ERROR("failed to compile input.");
    printf("\n");


    if(jet_flagreg_has_arg(&fr, JET_FLAG_PRINT, JET_FLAG_AST))
        jet_ast_print(&cu.ast);
    if(jet_flagreg_has_arg(&fr, JET_FLAG_PRINT, JET_FLAG_TOKENS))
        jet_token_table_print_da(&cu.tok_da);

    jet_cu_dispose(&cu);
    jet_flagreg_dispose(&fr);
}

static bool jet_compile(const char* filepath, jet_compilation_unit* cu)
{
    if(!cu) return false;
    if(!jet_cu_init(cu, filepath)) 
        return false;
    return jet_cu_run(cu);
}
