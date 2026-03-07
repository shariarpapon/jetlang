#pragma once
#include <jet_da.h>
#include <jet_token.h>
#include <jet_ast_node.h>

typedef struct jet_ast 
{
    jet_da tok_da;
    jet_da node_registry;
    jet_da top_nid_da;

    node_id prog_nid;
    size_t node_count;
    size_t tok_cursor;
} jet_ast;

typedef struct jet_parse
{
    jet_da* tok_da;
    size_t tok_cursor;
} jet_parser;


bool jet_ast_init(jet_ast* ast, const jet_da* tok_da);
void jet_ast_dispose(jet_ast* ast);
void jet_ast_reset(jet_ast* ast);
bool jet_ast_generate_nodes(jet_ast* ast);

const jet_ast_node* jet_ast_node_get(const jet_ast* ast, node_id nid);
const jet_da* jet_ast_get_top_nid_da(const jet_ast* ast);
node_id jet_ast_get_prog_nid(const jet_ast* ast);




/*
struct jet_ast 
{
    const jet_da tok_da;
    jet_da node_registry;
    jet_da top_nid_da;

    node_id prog_nid;
    size_t node_count;
    size_t tok_cursor;
};
*/

