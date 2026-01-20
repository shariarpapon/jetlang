#pragma once
#include <jet_list.h>
#include <jet_token.h>
#include <jet_ast_node.h>

typedef struct jet_ast 
{
   jet_list* tok_list;
   jet_list* node_list;
   jet_ast_node* prog_node;
   size_t tok_cursor;
} jet_ast;

jet_ast* jet_ast_create(jet_list* tok_list);
bool jet_ast_dispose(jet_ast* ast);
