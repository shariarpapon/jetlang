#pragma once
#include <stdbool.h>
#include <jet_span.h>
#include <jet_ast_node.h>
#include <jet_token.h>

typedef enum jet_error_kind jet_error_kind;

typedef struct jet_error jet_error;

// @parser
typedef struct jet_error_unexpected_token jet_error_unexpected_token;
typedef struct jet_error_expected_token jet_error_expected_token;
typedef struct jet_error_parsing_failed jet_error_parsing_failed;
typedef struct jet_error_invalid_token jet_error_invalid_token;

// @semantics
typedef struct jet_error_node_error jet_error_node_error;
typedef struct jet_error_type_redef jet_error_type_redef;

enum jet_error_kind
{
    JET_ERR_UNKNOWN,

    // @parser
    JET_ERR_UNEXPECTED_EOF,
    JET_ERR_UNEXPECTED_TOKEN,
    JET_ERR_EXPECTED_TOKEN,
    JET_ERR_INVALID_TOKEN,
    JET_ERR_PARSING_FAILED,
    JET_ERR_EXPECTED_EXPR,
    JET_ERR_PRIMARY_EXPR_PARSING_FAILED,
    JET_ERR_NO_VALID_PARSING_SEQ,
     
    // @semantics
    JET_ERR_NODE_NOT_ALLOWED,
    JET_ERR_TYPE_REDEF,
};

struct jet_error_invalid_token
{
    const jet_token* inv_tok;
};

struct jet_error_parsing_failed
{
    jet_ast_node_type node_type;
};

struct jet_error_unexpected_token
{
    jet_token_type found;
};

struct jet_error_expected_token
{
    jet_token_type found;
    jet_token_type expected;
};

struct jet_error_node_error
{
    jet_ast_node_type node_type;
};

struct jet_error_type_redef
{
    jet_ident type_name;
};

struct jet_error
{
    jet_error_kind kind;
    jet_span span;
    union
    {
        jet_error_unexpected_token unexpected_token;
        jet_error_expected_token expected_token;
        jet_error_parsing_failed parsing_failed;
        jet_error_invalid_token invalid_token;
        jet_error_node_error node_error; 
        jet_error_type_redef type_redef;
    } as;
};

jet_error jet_err(jet_error_kind kind, jet_span span);

// @parser
jet_error jet_err_invalid_token(const jet_token* inv_tok);
jet_error jet_err_unexpected_token(const jet_token* tok);
jet_error jet_err_expected_token(const jet_token* found_tok, jet_token_type expected_type);
jet_error jet_err_parsing_failed(jet_ast_node_type node_type, const jet_token* cursor_tok);

// @semantics
jet_error jet_err_node_error(jet_error_kind kind, const jet_ast_node* node);
jet_error jet_err_type_redef(const jet_ast_node* tdef_node);



