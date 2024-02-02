#ifndef LUA_EMBEDDED_EXAMPLE_LUA_EVALUATE_H
#define LUA_EMBEDDED_EXAMPLE_LUA_EVALUATE_H

#include <stdlib.h>
#include <string.h>
#include "lua536.h"

struct Result {
    long x;
    double answer;
    char *error_message;
};

typedef struct Result Result;

void evaluate_function(char *data_file_name, char *script_file_name);

int lua_evaluate_function(int dim, long *inputs, Result *outputs);

#endif
