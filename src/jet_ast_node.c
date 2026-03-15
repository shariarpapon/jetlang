#include <jet_ast_node.h>

#include <string.h>
#include <stdio.h>

bool jet_ast_node_init(jet_ast_node* node, jet_ast_node_type type, size_t start_cursor, size_t end_cursor)
{
    if(!node) return false;
    memset(node, 0, sizeof(*node));
    node->node_type = type;
    if(!jet_span_init(&node->span, start_cursor, end_cursor))
    {
        fprintf(stderr, "failed to init node, unable to init span.\n");
        return false;
    }
    return true;
}

bool jet_ast_node_dispose(jet_ast_node* node)
{
    //dispose according to type
    if(!node) 
        return false;

    switch(node->node_type)
    {
        default: 
            break;
        case AST_IDENT: 
        {
            if(node->as.ident.str)
                free(node->as.ident.str);
            break;
        }
        case AST_LIT:
        {
            if(node->as.lit.tkind == JET_TYPE_STR)
                free((void*)node->as.lit.as.s);
            break;
        }
        case AST_TYPE_DECL
        {
            free((void*)node->as.tdecl.tname);
            break;
        }       
    }

    return true;

}

const char* jet_ast_node_type_str(jet_ast_node_type node_type)
{
    switch(node_type)
    {
        default: 
            return "## ast_node_unrecognized ##";
        case AST_UNKNOWN:
            return "AST_UNKNOWN";
        case AST_PROG: 
            return "AST_PROG";
        case AST_MEM:
            return "AST_MEM";
        case AST_IDENT:
            return "AST_IDENT";
        case AST_LIT:
            return "AST_LIT";
        case AST_BLOCK:
            return "AST_BLOCK";
        case AST_VAR_DECL:
            return "AST_VAR_DECL";
        case AST_TYPE_DECL:
            return "AST_TYPE_DECL";
        case AST_FUNC_DECL:
            return "AST_FUNC_DECL";
        case AST_FUNC_DEF:
            return "AST_FUNC_DEF";
        case AST_CALL:
            return "AST_CALL";
        case AST_BINOP:
            return "AST_BINOP";
        case AST_UNOP:
            return "AST_UNOP";
    }    
}
