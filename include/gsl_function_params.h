#pragma once

#include <sol_include.h>

struct GslFunctionParams {
	sol::function func;
	double alpha = 1.0;
};
