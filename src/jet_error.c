#include <string.h>
#include <jet_error.h>
#include <jet_logger.h>

jet_error jet_err(jet_error_kind kind, jet_span span)
{
    jet_error error = {.kind = kind, .span = span };
    return error;
} 

jet_error jet_err_invalid_token(const jet_token* inv_tok)
{
    JET_ASSERT(inv_tok != NULL);
    jet_error err = jet_err(JET_ERR_INVALID_TOKEN, inv_tok->span);
    err.as.invalid_token = (jet_error_invalid_token){.inv_tok = inv_tok};
    return err;
}

jet_error jet_err_unexpected_token(const jet_token* tok)
{
    JET_ASSERT(tok != NULL);
    jet_error err = jet_err(JET_ERR_UNEXPECTED_TOKEN, tok->span);
    err.as.unexpected_token = (jet_error_unexpected_token){.found = tok->type };
    return err;
}

jet_error jet_err_expected_token(const jet_token* found_tok, jet_token_type expected_type)
{
    JET_ASSERT(found_tok != NULL);
    jet_error err = jet_err(JET_ERR_EXPECTED_TOKEN, found_tok->span);
    err.as.expected_token = (jet_error_expected_token){.found = found_tok->type, .expected = expected_type };
    return err;
}

jet_error jet_err_parsing_failed(jet_ast_node_type node_type, const jet_token* cursor_tok)
{
    JET_ASSERT(cursor_tok != NULL);
    jet_error err = jet_err(JET_ERR_PARSING_FAILED, cursor_tok->span);
    err.as.parsing_failed = (jet_error_parsing_failed){.node_type = node_type};
    return err;
}

jet_error jet_err_node_error(jet_error_kind kind, const jet_ast_node* node)
{
    JET_ASSERT(node != NULL);
    jet_error err = jet_err(kind, node->span);
    err.as.node_error = (jet_error_node_error){.node_type = node->node_type};
    return err;
}

jet_error jet_err_type_redef(const jet_ast_node* tdef_node)
{
    JET_ASSERT(tdef_node != NULL);
    jet_error err = jet_err(JET_ERR_TYPE_REDEF, tdef_node->span);
    err.as.type_redef = (jet_error_type_redef){.type_name = tdef_node->as.tdef.type_name};
    return err;
}
