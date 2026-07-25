#include "gwbasic.h"

bool io_save_program(InterpreterState *state, const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) return false;

    Program *prog = &state->program;
    for (size_t i = 0; i < prog->count; i++) {
        fprintf(f, "%u %s\n", prog->lines[i].line_num, prog->lines[i].code);
    }

    fclose(f);
    return true;
}

bool io_load_program(InterpreterState *state, const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return false;

    free_interpreter(state);
    init_interpreter(state);

    char buf[MAX_LINE_LEN];
    while (fgets(buf, sizeof(buf), f)) {
        // Strip trailing newline
        size_t len = strlen(buf);
        while (len > 0 && (buf[len - 1] == '\n' || buf[len - 1] == '\r')) {
            buf[--len] = '\0';
        }
        if (len > 0) {
            execute_line(state, buf);
        }
    }

    fclose(f);
    return true;
}
