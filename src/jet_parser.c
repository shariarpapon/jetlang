#include <stdlib.h>
#include <string.h>

#include <jet_parser_internals.h>
#include <jet_ident.h>
#include <jet_logger.h>

#define JET_PARSER_INIT_ERR_CAP (64)
#define JET_PARSER_INIT_TYPE_NAME_CAP (32)

#define ARRAY_LEN(a) (sizeof(a) / sizeof((a)[0]))

bool jet_parser_init(jet_parser* p, jet_compilation_unit* cu)
{
    if(p == NULL || cu == NULL)
        return false;

    memset(p, 0 , sizeof(*p)); 
    p->tok_cursor = 0;
    p->filename = cu->filename;
    p->tok_da = &cu->tok_da;

    //init
    if(!jet_ast_init(&cu->ast))
        return false;
    p->ast = &cu->ast;

    if(!jet_da_init(&cu->parser_err_da, JET_PARSER_INIT_ERR_CAP, sizeof(jet_error)))
        return false;
    p->err_da = &cu->parser_err_da;

    if(!jet_ident_hashset_init(&cu->type_name_hs, JET_PARSER_INIT_TYPE_NAME_CAP))
        return false;
    p->type_name_hs = &cu->type_name_hs;

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
        JET_ASSERT(result.kind == JET_RESULT_NID || result.kind == JET_RESULT_ERROR);
        if(result.kind == JET_RESULT_NID)
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





