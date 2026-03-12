#include <jet_ast_print.h>
#include <jet_print.h>
#include <jet_sb.h>
#include <jet_arena.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define JET_ASTP_COLS_CAP (16)
#define JET_ASTP_SB_CAP (64)

#define JET_AST_FILL "  "
#define JET_ASTP_VLINK "| "
#define JET_ASTP_HLINK "|_"

typedef struct jet_ast_printer
{
    const jet_ast* ast;
    jet_arena arena;
    jet_da active_cols;
    jet_sb sb;
} jet_ast_printer;

static bool jet_astp_init(jet_ast_printer* p, const jet_ast* ast);
static void jet_astp_reset(jet_ast_printer* p);
static void jet_astp_dispose(jet_ast_printer* p);
static void jet_astp_indent(jet_ast_printer* p, size_t depth);
static bool jet_astp_is_col_active(jet_ast_printer* p, size_t col);
static size_t jet_astp_add_col(jet_ast_printer* p, size_t col);
static void jet_astp_remove_col(jet_ast_printer* p, size_t col_id);
static const char* jet_astp_resolve_node_str(jet_ast_printer* p, const jet_ast_node* n);
static void jet_astp_print_node(jet_ast_printer* p, const jet_ast_node* node, size_t depth);
static void jet_astp_print_by_nid(jet_ast_printer* p, size_t nid, size_t depth);
static void jet_astp_print_nid_da(jet_ast_printer* p, const char* label, const jet_da* nid_da, size_t depth);
static bool jet_astp_init(jet_ast_printer* p, const jet_ast* ast)
{
    if(!p || !ast) return false;
    memset(p, 0, sizeof(*p));
    p->ast = ast;
    
    bool col_da_init = false;
    bool sb_init = false;
    bool arena_init = false;
    
    col_da_init = jet_da_init(&p->active_cols, JET_ASTP_COLS_CAP, sizeof(size_t));
    sb_init = jet_sb_init(&p->sb, JET_ASTP_SB_CAP);
    arena_init = jet_arena_init(&p->arena, 256);

    if(!col_da_init || !sb_init || !arena_init)
        goto fail;

    return true;

fail:
    if(col_da_init) jet_da_dispose(&p->active_cols);
    if(sb_init) jet_sb_dispose(&p->sb);
    if(arena_init) jet_arena_dispose(&p->arena);
    return false;
}

static void jet_astp_reset(jet_ast_printer* p)
{
    assert(p != NULL && "cannot reset jet_ast_printer, arg p is null.");
    jet_da_clear(&p->active_cols);
    jet_sb_clear(&p->sb);
    jet_arena_clear(&p->arena);
}

static void jet_astp_dispose(jet_ast_printer* p)
{
    if(!p) return;
    jet_da_dispose(&p->active_cols);
    jet_sb_dispose(&p->sb);
    jet_arena_dispose(&p->arena);
    memset(p, 0, sizeof(*p));
}

static void jet_astp_indent(jet_ast_printer* p, size_t depth)
{
    for(size_t i = 0; i < depth; i++)
    {
        if(jet_astp_is_col_active(p, i))
            ANSI_PRINTF(ANSI_GREEN2, "%s", JET_ASTP_VLINK);
        else 
            ANSI_PRINTF(ANSI_GREY1, "%s", JET_AST_FILL);
    }
}

static size_t jet_astp_add_col(jet_ast_printer* p, size_t col)
{
    assert(p != NULL && "err: cannot add column to jet_ast_printer, arg p is null.");
    assert(jet_da_append(&p->active_cols, (const void*)&col) && "err: cannot add column to jet_ast_printer, failed to append to da.\n");
    size_t index = jet_da_count(&p->active_cols) - 1;
    return index;
}

static void jet_astp_remove_col(jet_ast_printer* p, size_t col_id)
{
    assert(p != NULL && "err: cannot add column to jet_ast_printer, arg p is null.");
    jet_da_remove(&p->active_cols, col_id);
}

static bool jet_astp_is_col_active(jet_ast_printer* p, size_t col)
{
    assert(p != NULL && "cannot check if col is active, arg p is null.");
    for(size_t i = 0; i < jet_da_count(&p->active_cols); i++)
    {
        size_t* c = (size_t*)jet_da_get(&p->active_cols, i);
        if(c && *c == col)
            return true;
    }
    return false;
}

static const char* jet_astp_resolve_node_str(jet_ast_printer* p, const jet_ast_node* n)
{
    jet_sb_clear(&p->sb);
    const char* str = jet_ast_node_type_str(n->node_type);
    if(!str)
    {
        jet_sb_appendf(&p->sb, "NO_STR_CONV<%d>", (int)n->node_type);
        str = jet_sb_arena_dup(&p->sb, (void*)&p->arena, jet_arena_galloc);
    }
    jet_sb_clear(&p->sb);
    return str;
}

static void jet_astp_print_by_nid(jet_ast_printer* p, size_t nid, size_t depth)
{
    assert(p != NULL && "cannot print node by nid, arg p is null.");
    const jet_ast_node* n = jet_ast_node_get(p->ast, nid); 
    if(!n) return;
    jet_astp_print_node(p, n, depth);
}

static void jet_astp_print_node(jet_ast_printer* p, const jet_ast_node* n, size_t depth)
{
    if(!p || !n) return;

    const char* node_str = jet_astp_resolve_node_str(p, n); 
    size_t col_id = jet_astp_add_col(p, depth);
    size_t child_depth = depth + 1;
    
    jet_astp_indent(p, depth);
    ANSI_PRINTF(ANSI_GREEN2, "%s", JET_ASTP_HLINK);
    ANSI_PRINTF(ANSI_WHITE3, "%s\n", node_str);
    //print children
    switch(n->node_type)
    {
        case AST_PROG:      
            jet_astp_print_by_nid(p, n->as.prog.block_nid, child_depth);
            break;
        case AST_BLOCK:     
            jet_astp_print_nid_da(p, "[stmts]", &n->as.block.stmt_nid_da, child_depth);
            break;
        case AST_VAR_DECL:  
            jet_astp_print_by_nid(p, n->as.vdecl.tdecl_nid, child_depth);
            jet_astp_print_by_nid(p, n->as.vdecl.ident_nid, child_depth);
            jet_astp_print_by_nid(p, n->as.vdecl.init_value_nid, child_depth);
            break;
        case AST_FUNC_DECL:
            jet_astp_print_by_nid(p, n->as.fdecl.ident_nid, child_depth);
            jet_astp_print_nid_da(p, "[ret]", &n->as.fdecl.ret_tdecl_nid_da, child_depth);
            jet_astp_print_nid_da(p, "[params]", &n->as.fdecl.param_nid_da, child_depth);
            break;
        case AST_FUNC_DEF: 
            jet_astp_print_by_nid(p, n->as.fdef.fdecl_nid, child_depth);
            jet_astp_print_by_nid(p, n->as.fdef.block_nid, child_depth);
            break;
        case AST_CALL:      
            jet_astp_print_by_nid(p, n->as.call.callee_nid, child_depth);
            jet_astp_print_nid_da(p, "[args]", &n->as.call.arg_nid_da, child_depth);
            break;
        case AST_BINOP:     
            jet_astp_print_by_nid(p, n->as.binop.lhs_nid, child_depth);
            jet_astp_print_by_nid(p, n->as.binop.rhs_nid, child_depth);
            break;
        case AST_UNOP:      
            jet_astp_print_by_nid(p, n->as.unop.expr_nid, child_depth);
            break;
        default: 
            break;
    }
    jet_astp_remove_col(p, col_id);
}

static void jet_astp_print_nid_da(jet_ast_printer* p, const char* label, const jet_da* nid_da, size_t depth)
{
    if(!p || !nid_da) return;
    if(!label) label = "[collection]";

    size_t count = jet_da_count(nid_da);
    jet_astp_indent(p, depth);
    ANSI_PRINTF(ANSI_ORANGE1, "%s[%zu]\n", label, count);

    for(size_t i = 0; i < count; i++)
    {
        size_t* nid_ptr = (size_t*)jet_da_get(nid_da, i); 
        if(!nid_ptr) continue;
        const jet_ast_node* n = jet_ast_node_get(p->ast, *nid_ptr);
        if(!n) continue;
        jet_astp_print_node(p, n, depth + 1);
    }
}

// user entry
void jet_ast_print(const jet_ast* ast)
{
    jet_ast_printer p;
    bool init_success = jet_astp_init(&p, ast);
    if(!init_success)
    {
        ANSI_PRINTF(ANSI_RED1, "err: jet_ast_print.c/jet_ast_print - cannot print ast, failed to init printer.\n");
        return;
    }

    ANSI_PRINTF(ANSI_BLUE2, "\n[entry]\n");

    const jet_ast_node* prog = jet_ast_node_get(ast, ast->prog_nid);
    jet_astp_print_node(&p, prog, 0);
    jet_astp_reset(&p);

    ANSI_PRINTF(ANSI_GREY1, "\n=====================================\n");

    jet_astp_print_nid_da(&p, "[top_level]", &ast->top_nid_da, 0);
    
    jet_astp_dispose(&p);
}










