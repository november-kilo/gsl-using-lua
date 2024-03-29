#include "lua_integrate.h"

void print_lua_error() {
    printf("%s\n", lua_tostring(luaState, -1));
}

void print_integration_error(int status) {
    printf("%s\n", gsl_strerror(status));
}

void print_integration_result(double result) {
    printf("%g\n", result);
}

double calculate_function_value(double x, void *params) {
    lua_getglobal(luaState, "IntegrateFunction");
    lua_pushnumber(luaState, x);
    lua_pcall(luaState, 1, 1, 0);
    return lua_tonumber(luaState, -1);
}

void execute_lua_script_and_compute_integral(char *algorithm, char *script, int lower_limit, int upper_limit) {
    if (luaL_dofile(luaState, script) != LUA_OK) {
        print_lua_error();
        return;
    }

    gsl_set_error_handler_off();
    gsl_integration_workspace *workspace = gsl_integration_workspace_alloc(WORKSPACE_SIZE);

    int status;
    double integral_result, error;
    gsl_function integration_function;
    integration_function.function = &calculate_function_value;
    integration_function.params = NULL;

    if (strcmp("qags", algorithm) == 0) {
        status = gsl_integration_qags(&integration_function, lower_limit, upper_limit, INTEGRATION_ERROR,
                                      INTEGRATION_LIMIT, WORKSPACE_SIZE, workspace, &integral_result, &error);
        if (status) {
            print_integration_error(status);
        } else {
            print_integration_result(integral_result);
        }
    } else {
        printf("%s: %s\n", algorithm, strerror(EINVAL));
    }

    gsl_integration_workspace_free(workspace);
}
