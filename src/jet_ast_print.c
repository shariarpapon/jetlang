#include <jet_ast_print.h>
#include <jet_print.h>
#include <jet_sb.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define JET_ASTP_INDENT_WIDTH (4)
#define JET_ASTP_COLS_CAP (16)
#define JET_ASTP_SB_CAP (64)

typedef struct jet_ast_printer
{
    const jet_ast* ast;
    jet_da active_cols;
    jet_sb sb;
} jet_ast_printer;

static bool jet_astp_init(jet_ast_printer* p, const jet_ast* ast);
static void jet_astp_dispose(jet_ast_printer* p);
static size_t jet_astp_add_col(jet_ast_printer* p, size_t col);
static void jet_ast_print_node(jet_ast_printer* p, jet_ast_node* node, size_t depth);

static bool jet_astp_init(jet_ast_printer* p, const jet_ast* ast)
{
    if(!p || !ast) return false;
    memset(p, 0, sizeof(*p));
    p->ast = ast;
    if( !jet_da_init(&p->active_cols, JET_ASTP_COLS_CAP, sizeof(size_t)) )
        return false;

    if( !jet_sb_init(&p->sb, JET_ASTP_SB_CAP) )
    {
        jet_da_dispose(&p->active_cols);
        return false;
    }
    return true;
}

static void jet_astp_dispose(jet_ast_printer* p)
{
    if(!p) return;
    jet_da_dispose(&p->active_cols);
    jet_sb_dispose(&p->sb);
    memset(p, 0, sizeof(*p));
}

static size_t jet_astp_add_col(jet_ast_printer* p, size_t col)
{
    assert(p != NULL && "err: cannot add column to jet_ast_printer, arg p is null.");
    assert(jet_da_append(&p->active_cols, (const void*)&col) && "err: cannot add column to jet_ast_printer, failed to append to da.\n");
    size_t index = jet_da_count(&p->active_cols) - 1;
    return index;
}

static void jet_ast_print_node(jet_ast_printer* p, jet_ast_node* n, size_t depth)
{
    if(!p || !n) return;

    const char* type_str = jet_ast_node_type_str(n->node_type);
    size_t col_id = jet_astp_add_col(p, depth);
    size_t child_depth = depth + 1;

    switch(n->node_type)
    {
        case AST_PROG:      
            break;
        case AST_MEM:       
            break;
        case AST_IDENT:     
            break;
        case AST_LIT:       
            break;
        case AST_BLOCK:     
            break;
        case AST_VAR_DECL:  
            break;
        case AST_TYPE_DECL: 
            break;
        case AST_FUNC_DECL:
            break;
        case AST_FUNC_DEF: 
            break;
        case AST_CALL:      
            break;
        case AST_BINOP:     
            break;
        case AST_UNOP:      
            break;
        default:
            JET_CPRINT(JETC_RED, "printing logic for node_type: %s has not been implemented.\n", type_str);
            break;
    }
    assert(jet_da_remove(&p->active_cols, col_id) && "err: cannot continue printing ast, failed to remove column from da.");
}

// user entry
void jet_ast_print(const jet_ast* ast)
{
    jet_ast_printer p;
    bool init_success = jet_astp_init(&p, ast);
    if(!init_success)
    {
        JET_CPRINT(JETC_RED, "err: jet_ast_print.c/jet_ast_print - cannot print ast, failed to init printer.\n");
        return;
    }
    //todo: implement ast printing logic
    jet_astp_dispose(&p);
}










