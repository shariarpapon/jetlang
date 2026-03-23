#include <jet_ast.h>
#include <jet_logger.h>

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

// should be fresh ast init, otherwise memory leak will occure.
bool jet_ast_init(jet_ast* ast)
{
    JET_ASSERT(ast != NULL);
    memset(ast, 0, sizeof(*ast)); 
    ast->prog_nid = INVALID_NID;
    ast->node_count = 0;
    if(!jet_da_init(&ast->node_registry, 32, sizeof(jet_ast_node)) )
    {
        JET_LOG_FATAL("failed to init node_registry DA.");
        return false;
    }
    if( !jet_da_init(&ast->top_nid_da, 16, sizeof(node_id)) )
    {
        JET_LOG_FATAL("failed to init top_node_nid_da DA.");
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
    JET_ASSERT(ast != NULL);
    JET_ASSERT(node != NULL);
    if(!jet_da_append(&ast->node_registry, (const void*)node))
    {
        JET_LOG_ERROR(" failed to register node, unable to append.\n");
        abort();
        return INVALID_NID;
    }
    ast->node_count++;
    return ast->node_count;
}

bool jet_ast_push_nid(jet_ast* ast, node_id nid)
{
    JET_ASSERT(ast != NULL);
    const jet_ast_node* node = jet_ast_node_get(ast, nid);
    if(node == NULL)
    {
        JET_LOG_ERROR("nid=%zu does not exist in registry.", nid);
        return false;
    }
    if(node->node_type == AST_PROG)
    {
        if(ast->prog_nid == INVALID_NID)
            ast->prog_nid = nid;
        else
        {
            JET_LOG_ERROR("cannot push node to ast, multiple program entry points.");
            return false;
        }
    }
    else 
        jet_da_append(&ast->top_nid_da, (const void*)&nid);

    return true;
}

const jet_ast_node* jet_ast_node_get(const jet_ast* ast, node_id nid)
{
    JET_ASSERT(ast != NULL);
    if(nid == INVALID_NID)
    {
        JET_LOG_WARNING("id=%zu represents invalid node id.", (size_t)INVALID_NID);
        return NULL;
    }

    if(nid - 1 >= jet_da_count(&ast->node_registry))
    {
        JET_LOG_ERROR("cannot get node, index out of bounds.");
        return NULL;
    }

    // force nid - 1 >= 0, because 0 represents invalid.
    const jet_ast_node* node = (const jet_ast_node*)jet_da_get(&ast->node_registry, nid - 1);
    if(!node)
    {
        JET_LOG_ERROR("could not fetch node with id=%zu from registry, likely node this nid does not exist.", nid);
        return NULL;
    }
    return node;
}

const jet_da* jet_ast_get_top_nid_da(const jet_ast* ast)
{
    JET_ASSERT(ast != NULL);
    return (const jet_da*)&ast->top_nid_da;
}

node_id jet_ast_get_prog_nid(const jet_ast* ast)
{
    JET_ASSERT(ast != NULL);
    return ast->prog_nid;
}

















