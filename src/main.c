#include "gwbasic.h"

static void print_banner(void) {
    printf("GW-BASIC 3.23 (Linux Port)\n");
    printf("(C) Copyright Microsoft 1983, 1984, 1985, 1986, 1987, 1988.\n");
    printf("60000 Bytes free\n");
    printf("Ok\n");
}

static void print_usage(const char *prog_name) {
    printf("Usage: %s [OPTION]... [FILE]\n\n", prog_name);
    printf("GW-BASIC 3.23 (Linux Port) Interpreter\n\n");
    printf("Options:\n");
    printf("  -h, --help, -?    Display this help message and exit\n");
    printf("  --test <file>     Run test file and return error status exit code\n\n");
    printf("Arguments:\n");
    printf("  FILE              Execute BASIC program file directly\n");
}

int main(int argc, char *argv[]) {
    InterpreterState state;
    init_interpreter(&state);

    if (argc >= 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-?") == 0)) {
        print_usage(argv[0]);
        free_interpreter(&state);
        return 0;
    }

    if (argc >= 3 && strcmp(argv[1], "--test") == 0) {
        if (!io_load_program(&state, argv[2])) {
            fprintf(stderr, "Error loading test file: %s\n", argv[2]);
            return 1;
        }
        run_program(&state);
        int exit_status = state.has_error ? (state.error_line_num > 0 ? (state.error_line_num % 256) : 1) : state.exit_code;
        free_interpreter(&state);
        return exit_status;
    }

    if (argc >= 2) {
        if (io_load_program(&state, argv[1])) {
            run_program(&state);
            int exit_status = state.has_error ? (state.error_line_num > 0 ? (state.error_line_num % 256) : 1) : state.exit_code;
            free_interpreter(&state);
            return exit_status;
        } else {
            fprintf(stderr, "File not found: %s\n", argv[1]);
            free_interpreter(&state);
            return 1;
        }
    }

    print_banner();

    char line_buf[MAX_LINE_LEN];
    while (state.running) {
        if (!fgets(line_buf, sizeof(line_buf), stdin)) break;

        size_t len = strlen(line_buf);
        while (len > 0 && (line_buf[len - 1] == '\n' || line_buf[len - 1] == '\r')) {
            line_buf[--len] = '\0';
        }

        if (len > 0) {
            execute_line(&state, line_buf);
        }
        if (!state.running) break;
        printf("Ok\n");
    }

    int exit_status = state.has_error ? (state.error_line_num > 0 ? (state.error_line_num % 256) : 1) : state.exit_code;
    free_interpreter(&state);
    return exit_status;
}
