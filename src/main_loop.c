#include "main_loop.h"

void main_loop(void) {
    char *input;
    char algorithm[100];
    char data[100];
    char script[100];
    int a, b;

    while (1) {
        int valid_input = 0;
        input = readline("* ");

        if (strcmp(input, "q") == 0) {
            break;
        } else if (sscanf(input, "e %s %s", data, script) == 2) {
            valid_input = 1;
            evaluate_function(data, script);
        } else if (sscanf(input, "i %s %s %d %d", algorithm, script, &a, &b) == 4) {
            valid_input = 1;
            execute_lua_script_and_compute_integral(algorithm, script, a, b);
        }

        if (valid_input == 1) {
            add_history(input);
        }

        free(input);
    }
}
