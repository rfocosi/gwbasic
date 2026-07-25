#ifndef GWBASIC_H
#define GWBASIC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_LINE_LEN 256
#define MAX_VARS 256
#define MAX_LINE_NUM 65529

typedef enum {
    VAL_INT,
    VAL_SINGLE,
    VAL_DOUBLE,
    VAL_STRING
} ValueType;

typedef struct {
    ValueType type;
    union {
        int32_t i_val;
        float f_val;
        double d_val;
        char s_val[256];
    } val;
} Value;

typedef enum {
    TOK_NONE = 0,
    TOK_PRINT,
    TOK_LET,
    TOK_IF,
    TOK_THEN,
    TOK_ELSE,
    TOK_FOR,
    TOK_TO,
    TOK_STEP,
    TOK_NEXT,
    TOK_GOTO,
    TOK_GOSUB,
    TOK_RETURN,
    TOK_INPUT,
    TOK_REM,
    TOK_CLS,
    TOK_LOCATE,
    TOK_COLOR,
    TOK_END,
    TOK_LIST,
    TOK_RUN,
    TOK_NEW,
    TOK_SAVE,
    TOK_LOAD,
    TOK_SYSTEM,
    TOK_NUMBER,
    TOK_STRING_LIT,
    TOK_IDENTIFIER,
    TOK_PLUS,
    TOK_MINUS,
    TOK_MULT,
    TOK_DIV,
    TOK_MOD,
    TOK_POW,
    TOK_EQ,
    TOK_NE,
    TOK_LT,
    TOK_GT,
    TOK_LE,
    TOK_GE,
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_COMMA,
    TOK_SEMICOLON,
    TOK_COLON,
    TOK_EOF
} TokenType;

typedef struct {
    TokenType type;
    char text[MAX_LINE_LEN];
    Value number_val;
} Token;

typedef struct {
    uint16_t line_num;
    char code[MAX_LINE_LEN];
} ProgramLine;

typedef struct {
    ProgramLine *lines;
    size_t count;
    size_t capacity;
} Program;

typedef struct {
    char name[32];
    Value val;
} Variable;

typedef struct {
    Variable vars[MAX_VARS];
    size_t var_count;
    Program program;
    bool running;
    size_t current_line_idx;
    uint16_t gosub_stack[128];
    size_t gosub_top;
} InterpreterState;

// Function declarations
void init_interpreter(InterpreterState *state);
void free_interpreter(InterpreterState *state);

// Tokenizer & Lexer
Token *tokenize_line(const char *input, size_t *token_count);

// Runtime & Commands
void execute_line(InterpreterState *state, const char *line_text);
void run_program(InterpreterState *state);
void list_program(InterpreterState *state);

// Evaluator
Value eval_expression(InterpreterState *state, Token *tokens, size_t count, size_t *idx);
void set_variable(InterpreterState *state, const char *name, Value val);
Value get_variable(InterpreterState *state, const char *name);

// IO Screen & Terminal
void io_init_screen(void);
void io_close_screen(void);
void io_cls(void);
void io_locate(int row, int col);
void io_color(int fg, int bg);

// IO Disk
bool io_save_program(InterpreterState *state, const char *filename);
bool io_load_program(InterpreterState *state, const char *filename);

#endif // GWBASIC_H
