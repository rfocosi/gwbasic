#include "gwbasic.h"

static void print_banner(void) {
    printf("GW-BASIC 3.23 (Linux Port)\n");
    printf("(C) Copyright Microsoft 1983, 1984, 1985, 1986, 1987, 1988.\n");
    printf("60000 Bytes free\n");
    printf("Ok\n");
}

int main(int argc, char *argv[]) {
    InterpreterState state;
    init_interpreter(&state);

    if (argc >= 3 && strcmp(argv[1], "--test") == 0) {
        if (!io_load_program(&state, argv[2])) {
            fprintf(stderr, "Error loading test file: %s\n", argv[2]);
            return 1;
        }
        run_program(&state);
        free_interpreter(&state);
        return 0;
    }

    if (argc >= 2) {
        if (io_load_program(&state, argv[1])) {
            run_program(&state);
        } else {
            fprintf(stderr, "File not found: %s\n", argv[1]);
        }
        free_interpreter(&state);
        return 0;
    }

    print_banner();

    char line_buf[MAX_LINE_LEN];
    while (true) {
        if (!fgets(line_buf, sizeof(line_buf), stdin)) break;

        size_t len = strlen(line_buf);
        while (len > 0 && (line_buf[len - 1] == '\n' || line_buf[len - 1] == '\r')) {
            line_buf[--len] = '\0';
        }

        if (len > 0) {
            execute_line(&state, line_buf);
        }
        printf("Ok\n");
    }

    free_interpreter(&state);
    return 0;
}
