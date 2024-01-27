#include "lua_evaluate.h"

int count_lines(FILE *file) {
    int lines = 0;

    char buffer[255];
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        lines++;
    }
    rewind(file);

    return lines;
}

void read_function_inputs(FILE *file, long *inputs) {
    char buffer[10];
    char *p;
    int i = 0;

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        inputs[i++] = strtol(buffer, &p, 10);
    }

    fclose(file);
}

int lua_evaluate_function(int dim, long *inputs, Result *outputs) {
    int has_error = 0;

    memset(outputs, 0, dim * sizeof(Result));

    for (int i = 0; i < dim; i++) {
        outputs->x = inputs[i];
        lua_getglobal(luaState, "EvaluateFunction");
        lua_pushinteger(luaState, inputs[i]);
        lua_pcall(luaState, 1, 1, 0);

        if (lua_isnumber(luaState, -1)) {
            lua_Number result = lua_tonumber(luaState, -1);
            outputs->answer = result;
        } else {
            outputs->error_message = strdup(lua_tostring(luaState, -1));
            has_error = 1;
        }

        lua_pop(luaState, -1);
        outputs++;
    }

    return has_error;
}

void evaluate_function(char *data_file_name, char *script_file_name) {
    FILE *data_file = fopen(data_file_name, "r");
    if (data_file == NULL) {
        printw("Failed to open data file.\n");
    } else {
        if (luaL_dofile(luaState, script_file_name) != LUA_OK) {
            printw("%s\n", lua_tostring(luaState, -1));
        } else {
            int n = count_lines(data_file);
            Result function_outputs[n];
            long function_inputs[n];
            read_function_inputs(data_file, function_inputs);
            lua_evaluate_function(n, function_inputs, function_outputs);

            for (int i = 0; i < n; i++) {
                if (function_outputs[i].error_message != NULL) {
                    printw("f(%l) = %s\n", function_outputs[i].x, function_outputs[i].error_message);;
                    free(function_outputs[i].error_message);
                } else {
                    printw("f(%l) = %g\n", function_outputs[i].x, function_outputs[i].answer);
                }
            }
        }
    }
}
