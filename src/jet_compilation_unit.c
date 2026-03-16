#include <jet_compilation_unit.h>
#include <jet_io.h>

#include <jet_config.h>
#include <jet_token_print.h>
#include <jet_ast_print.h>

#include <string.h>
#include <stdio.h>

#define JET_CU_ARENA_CAP (1024 * 4)
#define JET_CU_TOK_CAP (64)

bool jet_cu_init(jet_compilation_unit* cu, const char* filename)
{
   if(!cu || !filename) 
       return false; 

   memset(cu, 0, sizeof(*cu));
   cu->filename = filename;
   cu->source_len = 0; 
   
   bool arena_init = false;
   bool tok_da_init = false;
   bool ast_init = false;

   arena_init = jet_arena_init(&cu->arena, JET_CU_ARENA_CAP);
   tok_da_init = jet_da_init(&cu->tok_da, JET_CU_TOK_CAP, sizeof(jet_token));
   ast_init = jet_ast_init(&cu->ast);
   cu->source = jet_io_read_text(filename, &cu->source_len);
   if(!cu->source || !arena_init || 
      !tok_da_init || !ast_init)
   {
       goto fail;
   }
   return true;

fail:
   if(cu->source) free((void*)cu->source);
   if(arena_init) jet_arena_dispose(&cu->arena);
   if(tok_da_init) jet_da_dispose(&cu->tok_da);
   if(ast_init) jet_ast_dispose(&cu->ast);
   return false;
}

void jet_cu_dispose(jet_compilation_unit* cu)
{
    if(!cu) return;
    jet_ast_dispose(&cu->ast);
    jet_da_dispose(&cu->tok_da); 
    if(cu->source) 
        free((void*)cu->source);
    jet_arena_dispose(&cu->arena);
    jet_ast_dispose(&cu->ast);
    memset(cu, 0, sizeof(*cu));
}

bool jet_cu_run(jet_compilation_unit* cu)
{
    if(!cu)
    {
        fprintf(stderr, "err: failed to compile, arg <cu> is null.\n");
        return false;
    } 

    jet_lexer lexer;
    jet_parser parser;

    bool lexer_init = false;
    lexer_init = jet_lexer_init(&lexer, cu->source, &cu->tok_da);
    if(!lexer_init || !jet_lexer_tokenize(&lexer)) 
        goto fail;

#ifdef JET_DBG_TPRINT_TOKENS
    jet_token_tprint_da(&cu->tok_da);
#endif

    bool parser_init = false;
    parser_init = jet_parser_init(&parser, (const jet_da*)&cu->tok_da, &cu->ast);
    if(!parser_init || !jet_parser_parse(&parser))
        goto fail;

#ifdef JET_DBG_PRINT_AST
    jet_ast_print(&cu->ast);
#endif

    jet_lexer_dispose(&lexer);
    jet_parser_dispose(&parser);
    return true;

fail:
    fprintf(stderr, "err: abrupt compilation failure.\n");
    if(lexer_init) jet_lexer_dispose(&lexer);
    if(parser_init) jet_parser_dispose(&parser);
    return false;
}

