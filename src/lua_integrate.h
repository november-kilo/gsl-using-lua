#ifndef LUA_EMBEDDED_EXAMPLE_LUA_INTEGRATE_H
#define LUA_EMBEDDED_EXAMPLE_LUA_INTEGRATE_H

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_integration.h>
#include "lua536.h"

#define WORKSPACE_SIZE 1000
#define INTEGRATION_LIMIT 1e-7
#define INTEGRATION_ERROR 0

void execute_lua_script_and_compute_integral(char *algorithm, char *script, int a, int b);

#endif
