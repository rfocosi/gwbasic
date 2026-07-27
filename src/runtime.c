#include "gwbasic.h"

void init_interpreter(InterpreterState *state) {
    memset(state, 0, sizeof(InterpreterState));
    state->program.lines = NULL;
    state->program.count = 0;
    state->program.capacity = 0;
    state->running = true;
    state->current_line_idx = 0;
    state->current_line_num = 0;
    state->has_error = false;
    state->error_line_num = 0;
    state->exit_code = 0;
    state->gosub_top = 0;
}

void free_interpreter(InterpreterState *state) {
    if (state->program.lines) {
        free(state->program.lines);
        state->program.lines = NULL;
    }
    state->program.count = 0;
    state->program.capacity = 0;
}

static void add_or_update_line(InterpreterState *state, uint16_t line_num, const char *code) {
    Program *prog = &state->program;
    
    // Check if line exists to update or delete
    for (size_t i = 0; i < prog->count; i++) {
        if (prog->lines[i].line_num == line_num) {
            if (strlen(code) == 0) { // Delete line
                for (size_t j = i; j < prog->count - 1; j++) {
                    prog->lines[j] = prog->lines[j + 1];
                }
                prog->count--;
            } else { // Update line
                strncpy(prog->lines[i].code, code, MAX_LINE_LEN - 1);
                prog->lines[i].code[MAX_LINE_LEN - 1] = '\0';
            }
            return;
        }
    }

    if (strlen(code) == 0) return; // Line didn't exist

    // Insert line in sorted order
    if (prog->count >= prog->capacity) {
        prog->capacity = (prog->capacity == 0) ? 16 : prog->capacity * 2;
        prog->lines = realloc(prog->lines, prog->capacity * sizeof(ProgramLine));
    }

    size_t insert_pos = 0;
    while (insert_pos < prog->count && prog->lines[insert_pos].line_num < line_num) {
        insert_pos++;
    }

    for (size_t j = prog->count; j > insert_pos; j--) {
        prog->lines[j] = prog->lines[j - 1];
    }

    prog->lines[insert_pos].line_num = line_num;
    strncpy(prog->lines[insert_pos].code, code, MAX_LINE_LEN - 1);
    prog->lines[insert_pos].code[MAX_LINE_LEN - 1] = '\0';
    prog->count++;
}

void list_program(InterpreterState *state) {
    Program *prog = &state->program;
    for (size_t i = 0; i < prog->count; i++) {
        printf("%u %s\n", prog->lines[i].line_num, prog->lines[i].code);
    }
}

void execute_line(InterpreterState *state, const char *line_text) {
    size_t token_count = 0;
    Token *tokens = tokenize_line(line_text, &token_count);
    if (token_count == 0) {
        free(tokens);
        return;
    }

    size_t idx = 0;

    // Check if line starts with line number
    if (tokens[idx].type == TOK_NUMBER && tokens[idx].number_val.type == VAL_INT) {
        uint16_t line_num = (uint16_t)tokens[idx].number_val.val.i_val;
        idx++;
        // Reconstruct remaining code
        const char *rest = strchr(line_text, ' ');
        while (rest && *rest == ' ') rest++;
        add_or_update_line(state, line_num, rest ? rest : "");
        free(tokens);
        return;
    }

    // Direct command execution
    switch (tokens[idx].type) {
        case TOK_PRINT: {
            idx++;
            bool trailing_semicolon = false;
            while (idx < token_count) {
                if (tokens[idx].type == TOK_SEMICOLON) {
                    trailing_semicolon = true;
                    idx++;
                    continue;
                }
                if (tokens[idx].type == TOK_COMMA) {
                    printf("\t");
                    idx++;
                    continue;
                }
                Value v = eval_expression(state, tokens, token_count, &idx);
                if (v.type == VAL_INT) printf("%d", v.val.i_val);
                else if (v.type == VAL_SINGLE) printf("%g", v.val.f_val);
                else if (v.type == VAL_DOUBLE) printf("%g", v.val.d_val);
                else if (v.type == VAL_STRING) printf("%s", v.val.s_val);
                trailing_semicolon = false;
            }
            if (!trailing_semicolon) printf("\n");
            break;
        }

        case TOK_LET:
        case TOK_IDENTIFIER: {
            if (tokens[idx].type == TOK_LET) idx++;
            if (tokens[idx].type == TOK_IDENTIFIER) {
                char var_name[32];
                strcpy(var_name, tokens[idx].text);
                idx++;
                if (idx < token_count && tokens[idx].type == TOK_EQ) {
                    idx++;
                    Value val = eval_expression(state, tokens, token_count, &idx);
                    set_variable(state, var_name, val);
                } else {
                    state->has_error = true;
                    state->error_line_num = state->current_line_num;
                    state->running = false;
                }
            } else {
                state->has_error = true;
                state->error_line_num = state->current_line_num;
                state->running = false;
            }
            break;
        }

        case TOK_CLS:
            io_cls();
            break;

        case TOK_LOCATE: {
            idx++;
            Value row = eval_expression(state, tokens, token_count, &idx);
            if (idx < token_count && tokens[idx].type == TOK_COMMA) idx++;
            Value col = eval_expression(state, tokens, token_count, &idx);
            io_locate(row.val.i_val, col.val.i_val);
            break;
        }

        case TOK_COLOR: {
            idx++;
            Value fg = eval_expression(state, tokens, token_count, &idx);
            if (idx < token_count && tokens[idx].type == TOK_COMMA) idx++;
            Value bg = eval_expression(state, tokens, token_count, &idx);
            io_color(fg.val.i_val, bg.val.i_val);
            break;
        }

        case TOK_LIST:
            list_program(state);
            break;

        case TOK_RUN:
            run_program(state);
            break;

        case TOK_NEW:
            free_interpreter(state);
            init_interpreter(state);
            break;

        case TOK_SAVE: {
            idx++;
            if (idx < token_count && (tokens[idx].type == TOK_STRING_LIT || tokens[idx].type == TOK_IDENTIFIER)) {
                io_save_program(state, tokens[idx].text);
            }
            break;
        }

        case TOK_LOAD: {
            idx++;
            if (idx < token_count && (tokens[idx].type == TOK_STRING_LIT || tokens[idx].type == TOK_IDENTIFIER)) {
                io_load_program(state, tokens[idx].text);
            }
            break;
        }

        case TOK_SYSTEM: {
            idx++;
            if (idx < token_count) {
                Value status_val = eval_expression(state, tokens, token_count, &idx);
                if (status_val.type == VAL_STRING) {
                    printf("%s\n", status_val.val.s_val);
                } else if (status_val.type == VAL_INT) {
                    printf("%d\n", status_val.val.i_val);
                } else if (status_val.type == VAL_SINGLE) {
                    printf("%g\n", status_val.val.f_val);
                } else if (status_val.type == VAL_DOUBLE) {
                    printf("%g\n", status_val.val.d_val);
                }
            }
            state->running = false;
            state->exit_code = 0;
            break;
        }

        case TOK_REM:
            break;

        case TOK_END:
            state->running = false;
            state->exit_code = 0;
            break;

        default:
            state->has_error = true;
            state->error_line_num = state->current_line_num;
            state->running = false;
            break;
    }

    free(tokens);
}

void run_program(InterpreterState *state) {
    state->running = true;
    state->current_line_idx = 0;
    state->has_error = false;
    state->error_line_num = 0;
    state->exit_code = 0;

    while (state->running && state->current_line_idx < state->program.count) {
        ProgramLine *line = &state->program.lines[state->current_line_idx];
        state->current_line_num = line->line_num;
        state->current_line_idx++;
        execute_line(state, line->code);
        if (state->has_error) {
            state->running = false;
            state->exit_code = state->error_line_num;
            break;
        }
    }
    state->running = false;
}
