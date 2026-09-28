#pragma once
#include <stdbool.h>
#include <jet_ast_node.h>
#include <jet_parsing_error.h>

typedef struct jet_result
{
    bool success;
    union
    {
        node_id nid;
        jet_parsing_error parsing_error;
    } as;
} jet_result;

bool jet_result_init(jet_result* result, bool success);


    


