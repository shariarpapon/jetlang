#include <jet_semantics_internals.h>
#include <jet_scope.h>
#include <jet_logger.h>
#include <jet_ast.h>

#define JET_SEMA_INIT_SCOPE_SYM_CAP (16)

typedef struct jet_sema_scope_id_hs_entry
{
    size_t nid;
    size_t scope_id;
} jet_sema_scope_id_hs_entry;

static uint64_t jet_sema_scope_id_hs_hash(const void* entry)
{
    const jet_sema_scope_id_hs_entry* e = entry;
    return (uint64_t)e->nid;
}

static bool jet_sema_scope_id_hs_equals(const void* a, const void* b)
{
    const jet_sema_scope_id_hs_entry* _a = a;
    const jet_sema_scope_id_hs_entry* _b = b;
    return _a->nid == _b->nid;
}

bool jet_sema_scope_id_hs_init(jet_hs* hs, size_t capacity)
{
    return jet_hs_init(hs, 
            capacity, 
            jet_sema_scope_id_hs_hash, 
            jet_sema_scope_id_hs_equals);
}

void jet_sema_scope_id_hs_add(jet_hs* hs, size_t nid, size_t scope_id)
{
    JET_ASSERT(hs != NULL);
    JET_ASSERT(nid != JET_INVALID_NID);
    JET_ASSERT(scope_id != JET_INVALID_SCOPEID);

    jet_sema_scope_id_hs_entry entry = 
    {
        .nid = nid,
        .scope_id = scope_id
    };

    JET_ASSERT(jet_hs_add(hs, &entry, sizeof(entry)) != NULL);
}

bool jet_sema_scope_id_hs_has_key(jet_hs* hs, size_t nid)
{
    JET_ASSERT(hs != NULL);
    return jet_hs_contains(hs, (const void*)&nid, NULL);
}

//returns the root scope_id
size_t jet_sema_register_node_scopes(jet_semantics* sema, size_t nid)
{
    JET_ASSERT(sema != NULL);
    JET_ASSERT(nid != JET_INVALID_NID);
    const jet_ast_node* node = jet_ast_get_node_by_nid(sema->ast, nid);
    JET_ASSERT(node != NULL);
    size_t scope_id = JET_INVALID_SCOPEID;
    switch(node->node_type)
    {
        default: 
            break;
        case AST_FUNC_DEF:
        {
            break;
        }
        case AST_BLOCK:
        {
            break;
        }

    }
    return scope_id;
}

bool jet_sema_construct_scope_from_node(jet_semantics* sema, size_t nid, size_t parent_scope_id, jet_scope* out_scope)
{
    JET_ASSERT(sema != NULL);
    JET_ASSERT(nid != JET_INVALID_NID);
    JET_ASSERT(out_scope != NULL);

    jet_scope scope = jet_scope_init_base(parent_scope_id);
    if(!jet_sema_generate_scope_symbols(sema, nid, &scope.sym_da))
        return false;
    *out_scope = scope;
    return true;
}

//takes the uninitialized jet_da
bool jet_sema_generate_scope_symbols(jet_semantics* sema, size_t nid, jet_da* out_da)
{
    JET_ASSERT(sema != NULL);
    JET_ASSERT(out_da != NULL);
    JET_ASSERT(nid != JET_INVALID_NID);
    
    const jet_ast_node* block_node = jet_ast_get_node_by_nid(sema->ast, nid);

    JET_ASSERT(block_node != NULL);
    if(block_node->node_type != AST_BLOCK)
        return false;

    JET_ASSERT(jet_da_init(out_da, JET_SEMA_INIT_SCOPE_SYM_CAP, sizeof(jet_symbol)));
    const jet_da* stmt_nid_da = &block_node->as.block.stmt_nid_da;
    size_t stmt_nid;
    jet_symbol sym = {0};
    for(size_t i = 0; i < jet_da_count(stmt_nid_da); i++)
    {
        stmt_nid = *((const size_t*)jet_da_get(stmt_nid_da, i));
        if(jet_sema_construct_scope_symbol_from_node(sema, stmt_nid, &sym))
            JET_ASSERT(jet_da_append(out_da, &sym));
    }
    return true;
}

bool jet_sema_construct_scope_symbol_from_node(jet_semantics* sema, size_t nid, jet_symbol* out_sym)
{
    JET_ASSERT(sema != NULL);
    JET_ASSERT(out_sym != NULL);
    JET_ASSERT(nid != JET_INVALID_NID);

    const jet_ast_node* node = jet_ast_get_node_by_nid(sema->ast, nid);
    JET_ASSERT(node != NULL);

    switch(node->node_type)
    {
        default: 
            return false;
        case AST_VAR_DECL:
            return jet_sema_construct_var_symbol(sema, nid, out_sym);
    }
}

