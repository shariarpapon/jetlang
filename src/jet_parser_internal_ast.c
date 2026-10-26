#include <jet_parser_internals.h>
#include <jet_logger.h>

jet_result jet_parser_register_node(jet_parser* p, const jet_ast_node* node)
{
    JET_ASSERT(p != NULL);
    JET_ASSERT(node != NULL);
    node_id nid = jet_ast_register_node(p->ast, node);
    if(nid == INVALID_NID)
    {
        JET_LOG_FATAL("failed to push node into ast, potentially overflowing/corrupted memory.");
        abort();
    }
    return jet_result_nid(nid);
}
