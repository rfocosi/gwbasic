#include "gwbasic.h"

typedef struct {
    const char *keyword;
    TokenType token;
} KeywordMap;

static KeywordMap keywords[] = {
    {"PRINT", TOK_PRINT},
    {"LET", TOK_LET},
    {"IF", TOK_IF},
    {"THEN", TOK_THEN},
    {"ELSE", TOK_ELSE},
    {"FOR", TOK_FOR},
    {"TO", TOK_TO},
    {"STEP", TOK_STEP},
    {"NEXT", TOK_NEXT},
    {"GOTO", TOK_GOTO},
    {"GOSUB", TOK_GOSUB},
    {"RETURN", TOK_RETURN},
    {"INPUT", TOK_INPUT},
    {"REM", TOK_REM},
    {"CLS", TOK_CLS},
    {"LOCATE", TOK_LOCATE},
    {"COLOR", TOK_COLOR},
    {"END", TOK_END},
    {"LIST", TOK_LIST},
    {"RUN", TOK_RUN},
    {"NEW", TOK_NEW},
    {"SAVE", TOK_SAVE},
    {"LOAD", TOK_LOAD},
    {"SYSTEM", TOK_SYSTEM},
    {NULL, TOK_NONE}
};

Token *tokenize_line(const char *input, size_t *token_count) {
    size_t capacity = 32;
    size_t count = 0;
    Token *tokens = malloc(capacity * sizeof(Token));
    
    size_t i = 0;
    size_t len = strlen(input);

    while (i < len) {
        while (i < len && isspace((unsigned char)input[i])) i++;
        if (i >= len) break;

        if (count >= capacity - 1) {
            capacity *= 2;
            tokens = realloc(tokens, capacity * sizeof(Token));
        }

        Token *tok = &tokens[count];
        memset(tok, 0, sizeof(Token));

        char c = input[i];

        if (c == '?') { // Shortcut for PRINT
            tok->type = TOK_PRINT;
            strcpy(tok->text, "PRINT");
            count++;
            i++;
            continue;
        }

        if (c == '"') { // String literal
            tok->type = TOK_STRING_LIT;
            size_t str_idx = 0;
            i++;
            while (i < len && input[i] != '"') {
                if (str_idx < sizeof(tok->number_val.val.s_val) - 1) {
                    tok->number_val.val.s_val[str_idx++] = input[i];
                }
                i++;
            }
            if (i < len && input[i] == '"') i++;
            tok->number_val.val.s_val[str_idx] = '\0';
            tok->number_val.type = VAL_STRING;
            strcpy(tok->text, tok->number_val.val.s_val);
            count++;
            continue;
        }

        if (isdigit((unsigned char)c) || (c == '.' && isdigit((unsigned char)input[i+1]))) {
            size_t start = i;
            bool is_float = false;
            while (i < len && (isdigit((unsigned char)input[i]) || input[i] == '.')) {
                if (input[i] == '.') is_float = true;
                i++;
            }
            char num_buf[64];
            size_t nlen = i - start;
            if (nlen >= sizeof(num_buf)) nlen = sizeof(num_buf) - 1;
            strncpy(num_buf, input + start, nlen);
            num_buf[nlen] = '\0';

            tok->type = TOK_NUMBER;
            strcpy(tok->text, num_buf);
            if (is_float) {
                tok->number_val.type = VAL_SINGLE;
                tok->number_val.val.f_val = atof(num_buf);
            } else {
                tok->number_val.type = VAL_INT;
                tok->number_val.val.i_val = atoi(num_buf);
            }
            count++;
            continue;
        }

        if (isalpha((unsigned char)c) || c == '_') {
            size_t start = i;
            while (i < len && (isalnum((unsigned char)input[i]) || input[i] == '_' || input[i] == '$' || input[i] == '%' || input[i] == '!')) {
                i++;
            }
            size_t wlen = i - start;
            char word[64];
            if (wlen >= sizeof(word)) wlen = sizeof(word) - 1;
            strncpy(word, input + start, wlen);
            word[wlen] = '\0';

            // Check if keyword
            char upper_word[64];
            for (size_t k = 0; k <= wlen; k++) upper_word[k] = toupper((unsigned char)word[k]);

            bool found_keyword = false;
            for (int k = 0; keywords[k].keyword != NULL; k++) {
                if (strcmp(upper_word, keywords[k].keyword) == 0) {
                    tok->type = keywords[k].token;
                    strcpy(tok->text, upper_word);
                    found_keyword = true;
                    break;
                }
            }

            if (!found_keyword) {
                tok->type = TOK_IDENTIFIER;
                strcpy(tok->text, upper_word);
            }
            count++;
            continue;
        }

        // Operators & punctuation
        switch (c) {
            case '+': tok->type = TOK_PLUS; strcpy(tok->text, "+"); i++; break;
            case '-': tok->type = TOK_MINUS; strcpy(tok->text, "-"); i++; break;
            case '*': tok->type = TOK_MULT; strcpy(tok->text, "*"); i++; break;
            case '/': tok->type = TOK_DIV; strcpy(tok->text, "/"); i++; break;
            case '^': tok->type = TOK_POW; strcpy(tok->text, "^"); i++; break;
            case '(': tok->type = TOK_LPAREN; strcpy(tok->text, "("); i++; break;
            case ')': tok->type = TOK_RPAREN; strcpy(tok->text, ")"); i++; break;
            case ',': tok->type = TOK_COMMA; strcpy(tok->text, ","); i++; break;
            case ';': tok->type = TOK_SEMICOLON; strcpy(tok->text, ";"); i++; break;
            case ':': tok->type = TOK_COLON; strcpy(tok->text, ":"); i++; break;
            case '=': tok->type = TOK_EQ; strcpy(tok->text, "="); i++; break;
            case '<':
                if (i + 1 < len && input[i+1] == '>') {
                    tok->type = TOK_NE; strcpy(tok->text, "<>"); i += 2;
                } else if (i + 1 < len && input[i+1] == '=') {
                    tok->type = TOK_LE; strcpy(tok->text, "<="); i += 2;
                } else {
                    tok->type = TOK_LT; strcpy(tok->text, "<"); i++;
                }
                break;
            case '>':
                if (i + 1 < len && input[i+1] == '=') {
                    tok->type = TOK_GE; strcpy(tok->text, ">="); i += 2;
                } else {
                    tok->type = TOK_GT; strcpy(tok->text, ">"); i++;
                }
                break;
            default:
                i++; // Skip unknown characters
                continue;
        }
        count++;
    }

    tokens[count].type = TOK_EOF;
    strcpy(tokens[count].text, "");
    *token_count = count;
    return tokens;
}
