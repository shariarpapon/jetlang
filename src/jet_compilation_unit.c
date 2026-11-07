#include <string.h>
#include <stdio.h>

#include <jet_compilation_unit.h>
#include <jet_io.h>
#include <jet_token.h>
#include <jet_parser.h>
#include <jet_lexer.h>
#include <jet_token_print.h>
#include <jet_ast_print.h>
#include <jet_logger.h>
#include <jet_error.h>
#include <jet_ident.h>
#include <jet_type.h>

#ifdef _WIN32
#include<windows.h>
#endif

#define JET_CU_ARENA_CAP (1024 * 4)
#define JET_CU_TOK_CAP (64)
#define JET_CU_ERROR_CAP (16)
#define JET_CU_TYPE_NAME_CAP (64)
#define JET_CU_TYPEDEF_CAP (32)

bool jet_cu_init(jet_compilation_unit* cu, const char* filename)
{
    if(!cu || !filename) 
        return false;

    memset(cu, 0, sizeof(*cu));
    cu->filename = filename;
    cu->source_len = 0; 
    
    bool arena_init = jet_arena_init(&cu->arena, JET_CU_ARENA_CAP);
    bool ast_init = jet_ast_init(&cu->ast);
    bool tok_da_init = jet_da_init(&cu->tok_da, JET_CU_TOK_CAP, sizeof(jet_token));
    bool errors_init = jet_da_init(&cu->parser_err_da, JET_CU_ERROR_CAP, sizeof(jet_error));
    bool type_name_hs_init = jet_ident_hashset_init(&cu->type_name_hs, JET_CU_TYPE_NAME_CAP);
    bool typedef_da_init = jet_da_init(&cu->typedef_da, JET_CU_TYPEDEF_CAP, sizeof(jet_typedef));

    cu->source = jet_io_read_text(filename, &cu->source_len);
    if(!cu->source || !arena_init || !tok_da_init 
       || !ast_init || !errors_init || !type_name_hs_init || !typedef_da_init)
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
    if(type_name_hs_init) jet_hs_dispose_full(&cu->type_name_hs);
    if(typedef_da_init) jet_da_dispose(&cu->typedef_da);
    return false;
}

void jet_cu_dispose(jet_compilation_unit* cu)
{
    if(!cu) return;
    if(cu->source) 
        free((void*)cu->source);
    jet_da_dispose(&cu->typedef_da);
    jet_hs_dispose_full(&cu->type_name_hs);
    jet_da_dispose(&cu->parser_err_da);
    jet_ast_dispose(&cu->ast);
    jet_da_dispose(&cu->tok_da); 
    jet_arena_dispose(&cu->arena);
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
    parser_init = jet_parser_init(&parser, cu);
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

