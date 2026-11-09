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
#include <jet_semantics.h>

#ifdef _WIN32
#include<windows.h>
#endif

#define JET_CU_ARENA_CAP (1024 * 2)

bool jet_cu_init(jet_compilation_unit* cu, const char* filename)
{
    if(!cu || !filename) 
        return false;

    memset(cu, 0, sizeof(*cu));
    
    cu->filename = filename;
    if(!jet_arena_init(&cu->arena, JET_CU_ARENA_CAP))
        return false;

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
}

void jet_cu_dispose(jet_compilation_unit* cu)
{
    if(!cu) return;
    jet_hs_dispose_full(&cu->type_id_hs);
    jet_da_dispose(&cu->type_da);
    jet_hs_dispose_full(&cu->type_name_hs);
    jet_da_dispose(&cu->parser_err_da);
    jet_ast_dispose(&cu->ast);
    jet_da_dispose(&cu->tok_da); 
    jet_arena_dispose(&cu->arena);
    if(cu->source) 
        free((void*)cu->source);
    memset(cu, 0, sizeof(*cu));
}

bool jet_cu_run(jet_compilation_unit* cu)
{
    JET_ASSERT(cu != NULL);    

    jet_lexer lexer;
    JET_ASSERT(jet_lexer_init(&lexer, cu));
    JET_LOG_INFO("lexer initialized");
    JET_ASSERT(jet_lexer_tokenize(&lexer));
    JET_LOG_INFO("lexer finished");

    jet_parser parser;
    JET_ASSERT(jet_parser_init(&parser, cu));
    JET_LOG_INFO("parser initialized");
    JET_ASSERT(jet_parser_parse(&parser));
    JET_LOG_INFO("parser finished");
    
    JET_ASSERT(jet_sema_init(cu));
    JET_LOG_INFO("semantics initialized");
    JET_ASSERT(jet_sema_resolve_types(cu)); 
    JET_LOG_INFO("semantics finished");

    jet_lexer_dispose(&lexer);
    jet_parser_dispose(&parser);
    return true;
}

