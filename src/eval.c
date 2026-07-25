#include "gwbasic.h"

void set_variable(InterpreterState *state, const char *name, Value val) {
    for (size_t i = 0; i < state->var_count; i++) {
        if (strcmp(state->vars[i].name, name) == 0) {
            state->vars[i].val = val;
            return;
        }
    }
    if (state->var_count < MAX_VARS) {
        strncpy(state->vars[state->var_count].name, name, 31);
        state->vars[state->var_count].name[31] = '\0';
        state->vars[state->var_count].val = val;
        state->var_count++;
    }
}

Value get_variable(InterpreterState *state, const char *name) {
    for (size_t i = 0; i < state->var_count; i++) {
        if (strcmp(state->vars[i].name, name) == 0) {
            return state->vars[i].val;
        }
    }
    Value default_val;
    if (strchr(name, '$') != NULL) {
        default_val.type = VAL_STRING;
        strcpy(default_val.val.s_val, "");
    } else {
        default_val.type = VAL_INT;
        default_val.val.i_val = 0;
    }
    return default_val;
}

static Value eval_primary(InterpreterState *state, Token *tokens, size_t count, size_t *idx) {
    Value res;
    memset(&res, 0, sizeof(Value));

    if (*idx >= count) return res;

    Token *tok = &tokens[*idx];

    if (tok->type == TOK_NUMBER) {
        (*idx)++;
        return tok->number_val;
    }

    if (tok->type == TOK_STRING_LIT) {
        (*idx)++;
        return tok->number_val;
    }

    if (tok->type == TOK_IDENTIFIER) {
        (*idx)++;
        return get_variable(state, tok->text);
    }

    if (tok->type == TOK_LPAREN) {
        (*idx)++;
        res = eval_expression(state, tokens, count, idx);
        if (*idx < count && tokens[*idx].type == TOK_RPAREN) {
            (*idx)++;
        }
        return res;
    }

    return res;
}

static Value eval_multiplicative(InterpreterState *state, Token *tokens, size_t count, size_t *idx) {
    Value left = eval_primary(state, tokens, count, idx);

    while (*idx < count) {
        TokenType op = tokens[*idx].type;
        if (op != TOK_MULT && op != TOK_DIV && op != TOK_MOD) break;

        (*idx)++;
        Value right = eval_primary(state, tokens, count, idx);

        if (left.type == VAL_INT && right.type == VAL_INT) {
            if (op == TOK_MULT) left.val.i_val *= right.val.i_val;
            else if (op == TOK_DIV) left.val.i_val = (right.val.i_val != 0) ? left.val.i_val / right.val.i_val : 0;
            else if (op == TOK_MOD) left.val.i_val = (right.val.i_val != 0) ? left.val.i_val % right.val.i_val : 0;
        } else {
            float l = (left.type == VAL_INT) ? (float)left.val.i_val : left.val.f_val;
            float r = (right.type == VAL_INT) ? (float)right.val.i_val : right.val.f_val;
            left.type = VAL_SINGLE;
            if (op == TOK_MULT) left.val.f_val = l * r;
            else if (op == TOK_DIV) left.val.f_val = (r != 0.0f) ? l / r : 0.0f;
        }
    }

    return left;
}

static Value eval_additive(InterpreterState *state, Token *tokens, size_t count, size_t *idx) {
    Value left = eval_multiplicative(state, tokens, count, idx);

    while (*idx < count) {
        TokenType op = tokens[*idx].type;
        if (op != TOK_PLUS && op != TOK_MINUS) break;

        (*idx)++;
        Value right = eval_multiplicative(state, tokens, count, idx);

        if (left.type == VAL_STRING || right.type == VAL_STRING) {
            if (op == TOK_PLUS) {
                left.type = VAL_STRING;
                strncat(left.val.s_val, right.val.s_val, sizeof(left.val.s_val) - strlen(left.val.s_val) - 1);
            }
        } else if (left.type == VAL_INT && right.type == VAL_INT) {
            if (op == TOK_PLUS) left.val.i_val += right.val.i_val;
            else if (op == TOK_MINUS) left.val.i_val -= right.val.i_val;
        } else {
            float l = (left.type == VAL_INT) ? (float)left.val.i_val : left.val.f_val;
            float r = (right.type == VAL_INT) ? (float)right.val.i_val : right.val.f_val;
            left.type = VAL_SINGLE;
            if (op == TOK_PLUS) left.val.f_val = l + r;
            else if (op == TOK_MINUS) left.val.f_val = l - r;
        }
    }

    return left;
}

Value eval_expression(InterpreterState *state, Token *tokens, size_t count, size_t *idx) {
    return eval_additive(state, tokens, count, idx);
}
