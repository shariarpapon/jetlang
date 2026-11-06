#include <stdlib.h>
#include <string.h>

#include <jet_parser_internals.h>
#include <jet_logger.h>

#define JET_PARSER_MAX_ERRORS (64)
#define ARRAY_LEN(a) (sizeof(a) / sizeof((a)[0]))

bool jet_parser_init(jet_parser* p, jet_compilation_unit* cu)
{
    JET_ASSERT(p != NULL);
    if(cu == NULL)
    {
        JET_LOG_ERROR("could not init parser, null param/s.");
        return false;
    }

    memset(p, 0 , sizeof(*p));
    p->filename = cu->filename;
    p->tok_da = &cu->tok_da;
    p->err_da = &cu->parser_err_da;
    p->type_name_hs = &cu->type_name_hs;
    p->ast = &cu->ast;
    p->tok_cursor = 0;

    //parser should be valid initialized state by this point
    jet_parser_add_primitive_type_names(p);
    return true;
}

bool jet_parser_dispose(jet_parser* p)
{
    if(!p) return false;
    memset(p, 0, sizeof(*p));
    return true;
}

bool jet_parser_parse(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    if(jet_da_count(p->tok_da) == 0)
        return true;

    jet_token_type t = TOK_EOF; 
    while(true)
    {
        t = jet_parser_peekn_tok_type(p, 0); 
        if(t == TOK_EOF) break; 
        jet_result result = jet_parser_parse_toplevel(p); 
        if(result.success == true)
            jet_ast_push_nid(p->ast, result.as.nid);
        else
        {
            jet_parser_push_err(p, (const jet_error*)&result.as.error);
            if(!jet_parser_sync_toplevel(p))
                return false;
        }
    }
    return true;
}





