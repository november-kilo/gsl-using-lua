#include "main_loop.h"

void main_loop(void) {
    char input[1000];
    char data[100];
    char script[100];
    int a, b;

    while (1) {
        attron(A_REVERSE);
        printw("\nAwaiting input: ");
        attroff(A_REVERSE);
        getstr(input);

        if (strcmp(input, "") == 0) {
            clear();
        } else if (strcmp(input, "q") == 0) {
            break;
        } else if (sscanf(input, "e %s %s", data, script) == 2) {
            clear();
            evaluate_function(data, script);
        } else if (sscanf(input, "i %s %d %d", script, &a, &b) == 3) {
            clear();
            execute_lua_script_and_compute_integral(script, a, b);
        }
    }
}
