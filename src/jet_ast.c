#include <jet_ast.h>

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

// should be fresh ast init, otherwise memory leak will occure.
bool jet_ast_init(jet_ast* ast)
{
    assert(ast != NULL && "cannot init, param ast is null");  
    memset(ast, 0, sizeof(*ast)); 
    ast->prog_nid = INVALID_NID;
    ast->node_count = 0;
    if( !jet_da_init(&ast->node_registry, 32, sizeof(jet_ast_node)) )
    {
        fprintf(stderr, "err: failed to init node_registry DA.\n");
        return false;
    }
    if( !jet_da_init(&ast->top_nid_da, 16, sizeof(node_id)) )
    {
        fprintf(stderr, "err: failed to init top_node_nid_da DA.\n");
        jet_da_dispose(&ast->node_registry);
        return false;
    } 
    return true;
}

void jet_ast_dispose(jet_ast* ast)
{
    if(!ast) return;
    
    for(size_t i = 0; i < jet_da_count(&ast->node_registry); i++)
    {
        void* node = jet_da_get(&ast->node_registry, i);
        if(node) 
            jet_ast_node_dispose((jet_ast_node*)node);
    }

    jet_da_dispose(&ast->node_registry);
    jet_da_dispose(&ast->top_nid_da);
    memset(ast, 0, sizeof(*ast)); 
}

node_id jet_ast_register_node(jet_ast* ast, const jet_ast_node* node)
{
    assert(ast != NULL && "cannot register node, ast is null");
    assert(node != NULL && "cannot register node, param node is null");
    if(!jet_da_append(&ast->node_registry, (const void*)node))
    {
        fprintf(stderr, "err: failed to register node, unable to append.\n");
        abort();
        return INVALID_NID;
    }
    ast->node_count++;
    return ast->node_count;
}

bool jet_ast_push_nid(jet_ast* ast, node_id nid)
{
    assert(ast != NULL && "cannot push nid, ast is null.");
    const jet_ast_node* node = jet_ast_node_get(ast, nid);
    if(node == NULL)
    {
        fprintf(stderr, "err: nid=%zu does not exist in registry.\n", nid);
        return false;
    }
    if(node->node_type == AST_PROG)
    {
        if(ast->prog_nid == INVALID_NID)
            ast->prog_nid = nid;
        else
        {
            fprintf(stderr, "error: cannot push node to ast, multiple program entry points.\n");
            return false;
        }
    }
    else 
        jet_da_append(&ast->top_nid_da, (const void*)&nid);

    return true;
}

const jet_ast_node* jet_ast_node_get(const jet_ast* ast, node_id nid)
{
    assert(ast != NULL && "cannot get node with specified nid, ast is null");
    if(nid == INVALID_NID)
    {
        fprintf(stderr, "err: cannot get node with id=%zu, this id internally represents invalid node.\n", (size_t)INVALID_NID);
        return NULL;
    }

    if(nid - 1 >= jet_da_count(&ast->node_registry))
    {
        fprintf(stderr, "err: cannot get node, index out of bounds.\n");
        return NULL;
    }

    // force nid - 1 >= 0, because 0 represents invalid.
    const jet_ast_node* node = (const jet_ast_node*)jet_da_get(&ast->node_registry, nid - 1);
    if(!node)
    {
        fprintf(stderr, "err: node with id=%zu does not exist in registry.\n", nid);
        return NULL;
    }
    return node;
}

const jet_da* jet_ast_get_top_nid_da(const jet_ast* ast)
{
    assert(ast != NULL && "cannot get top_nid_da, ast is null");
    return (const jet_da*)&ast->top_nid_da;
}

node_id jet_ast_get_prog_nid(const jet_ast* ast)
{
    assert(ast != NULL && "cannot get prog nid, ast is null");
    return ast->prog_nid;
}

















