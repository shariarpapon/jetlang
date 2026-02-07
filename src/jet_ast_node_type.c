#include <jet_ast_node_type.h>

const char* jet_ast_node_type_str(jet_ast_node_type node_type)
{
    switch(node_type)
    {
        default: 
            return "## ast_node_unrecognized ##";
        case AST_UNKNOWN:
            return "AST_UNKNOWN";
        case AST_MEM:
            return "AST_MEM";
        case AST_PROG: 
            return "AST_PROG";
        case AST_IDENT:
            return "AST_IDENT";
        case AST_LIT:
            return "AST_LIT";
        case AST_BLOCK:
            return "AST_BLOCK";
        case AST_VAR_REF:
            return "AST_VAR_REF";
        case AST_VAR_DECL:
            return "AST_VAR_DECL";
        case AST_TYPE_DECL:
            return "AST_TYPE_DECL";
        case AST_FUNC_DECL:
            return "AST_FUNC_DECL";
        case AST_FUNC_DEF:
            return "AST_FUNC_DEF";
        case AST_FUNC_CALL:
            return "AST_FUNC_CALL";
        case AST_BINOP:
            return "AST_BINOP";
        case AST_UNOP:
            return "AST_UNOP";
    }
    
}

