#include <jet_error.h>
#include <string.h>

jet_error jet_err(jet_error_kind kind, jet_span span)
{
    jet_error error;
    memset(&error, 0, sizeof(error));
    error.kind = kind;
    error.span = span;
    return error;
} 

jet_error jet_err_unexpected_token(jet_token* tok)
{
    ASSERT(tok != NULL);
    jet_error err = jet_err(JET_ERR_UNEXPECTED_TOKEN, tok->span);
    err.as.unexpected_token = {.found = tok->type };
    return err;
}

jet_error jet_err_expected_token(jet_token* found_tok, jet_token_type expected_type)
{
    ASSERT(found_tok != NULL);
    jet_error err = jet_err(JET_ERR_EXPECTED_TOKEN, found_tok->span);
    err.as.expected_token = {.found = found_tok->type, .expected = expected_type };
    return err;

}
