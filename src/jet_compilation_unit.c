#include <jet_compilation_unit.h>
#include <jet_io.h>
#include <jet_token_print.h>
#include <jet_ast_print.h>
#include <jet_logger.h>
#include <jet_error.h>
#include <jet_ident.h>

#include <string.h>
#include <stdio.h>

#ifdef _WIN32
#include<windows.h>
#endif

#define JET_CU_ARENA_CAP (1024 * 4)
#define JET_CU_TOK_CAP (64)
#define JET_CU_ERROR_CAP (16)
#define JET_CU_TDEF_NAME_HS_CAP (64)

bool jet_cu_init(jet_compilation_unit* cu, const char* filename)
{
    JET_ASSERT(cu != NULL);
    if(!filename) return false;

    memset(cu, 0, sizeof(*cu));
    cu->filename = filename;
    cu->source_len = 0; 

    bool arena_init = false;
    bool tok_da_init = false;
    bool ast_init = false;
    bool errors_init = false;
    bool defined_type_names_hs_init = false;

    arena_init = jet_arena_init(&cu->arena, JET_CU_ARENA_CAP);
    ast_init = jet_ast_init(&cu->ast);
    tok_da_init = jet_da_init(&cu->tok_da, JET_CU_TOK_CAP, sizeof(jet_token));
    errors_init = jet_da_init(&cu->parser_err_da, JET_CU_ERROR_CAP, sizeof(jet_error));
    defined_type_names_hs_init = jet_ident_hashset_init(&cu->defined_type_names_hs, JET_CU_TDEF_NAME_HS_CAP);

    cu->source = jet_io_read_text(filename, &cu->source_len);
    if(!cu->source || !arena_init || !tok_da_init 
       || !ast_init || !errors_init || !defined_type_names_hs_init)
    {
        goto fail;
    }

    //intialize ansi for windows 
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= 0x0004;
    SetConsoleMode(hOut, dwMode);
    ansi_initialized = true;
#endif

    return true;

fail:
    if(cu->source) free((void*)cu->source);
    if(arena_init) jet_arena_dispose(&cu->arena);
    if(tok_da_init) jet_da_dispose(&cu->tok_da);
    if(errors_init) jet_da_dispose(&cu->parser_err_da);
    if(ast_init) jet_ast_dispose(&cu->ast);
    if(defined_type_names_hs_init) jet_hs_dispose_full(&cu->defined_type_names_hs);
    return false;
}

void jet_cu_dispose(jet_compilation_unit* cu)
{
    if(!cu) return;
    if(cu->source) 
        free((void*)cu->source);
    jet_da_dispose(&cu->tok_da); 
    jet_da_dispose(&cu->parser_err_da);
    jet_arena_dispose(&cu->arena);
    jet_ast_dispose(&cu->ast);
    jet_hs_dispose_full(&cu->defined_type_names_hs);
    memset(cu, 0, sizeof(*cu));
}

bool jet_cu_run(jet_compilation_unit* cu)
{
    JET_ASSERT(cu != NULL);    

    jet_lexer lexer;
    jet_parser parser;

    bool lexer_init = false;
    lexer_init = jet_lexer_init(&lexer, cu->filename, cu->source, &cu->tok_da);
    if(!lexer_init || !jet_lexer_tokenize(&lexer)) 
    {
        JET_LOG_ERROR("lexer failed");
        goto fail;
    }

    bool parser_init = false;
    parser_init = jet_parser_init(&parser, cu->filename, (const jet_da*)&cu->tok_da, &cu->ast, &cu->parser_err_da, &cu->defined_type_names_hs);
    if(!parser_init || !jet_parser_parse(&parser))
    {
        JET_LOG_ERROR("parser failed");
        goto fail;
    }
    jet_lexer_dispose(&lexer);
    jet_parser_dispose(&parser);
    return true;

fail:
    JET_LOG_ERROR("abrupt compilation failure.");
    if(lexer_init) jet_lexer_dispose(&lexer);
    if(parser_init) jet_parser_dispose(&parser);
    return false;
}

