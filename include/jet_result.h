#pragma once
#include <stdbool.h>
#include <jet_ast_node.h>
#include <jet_error.h>

typedef struct jet_result
{
    bool success;
    union
    {
        node_id nid;
        jet_error lexer_error;
        jet_error parser_error;
        jet_error semantic_error;
    } as;
} jet_result;

bool jet_result_init(jet_result* result, bool success);
void jet_result_dispose(jet_result* result);


    


