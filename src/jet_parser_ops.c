#include <jet_parser_ops.h>

uint8_t jet_parser_get_op_prec(jet_token_type op_type)
{
    switch(op_type) {
        default: return PREC_NONE; // not an operator
        case TOK_INCR:
        case TOK_DECR:
        case TOK_DOT: return PREC_POSTFIX;
        case TOK_NOT: return PREC_PREFIX;
        
        case TOK_STAR:
        case TOK_SLASH:
        case TOK_MOD: return PREC_MULT;
        
        case TOK_PLUS:
        case TOK_MINUS: return PREC_ADDITIVE;
        
        case TOK_SHL:
        case TOK_SHR: return PREC_BIT_SHIFT;
        
        case TOK_BAND: return PREC_BAND;
        case TOK_BOR: return PREC_BOR;
        case TOK_AND: return PREC_AND;
        case TOK_OR: return PREC_OR;
        
        case TOK_GTE:
        case TOK_LTE:
        case TOK_GT:
        case TOK_LT: return PREC_RELATIONAL;

        case TOK_ASG:
        case TOK_PLUSEQ:
        case TOK_MINEQ:
        case TOK_MULEQ:
        case TOK_DIVEQ:
        case TOK_MODEQ:
        case TOK_XOREQ:
        case TOK_BANDEQ:
        case TOK_BOREQ: return PREC_ASG;
        
        case TOK_POW: return PREC_POW;

    }
}

