#pragma once

#include <gsl/gsl_errno.h>
#include <gsl/gsl_integration.h>
#include <filesystem>
#include <string>
#include "lua_util.h"
#include "imgui.h"

#define EPSILON 1e-7
#define LIMIT 5000

enum class IntegrationAlgorithm {
    qags,
    qagi,
    qagiu,
    qagil
};

namespace gsl_util {
    int integrate(const std::filesystem::path& path, const std::string& function_name,
                  IntegrationAlgorithm algorithm, double* integration_result,
                  double* absolute_error,
                  std::string& error_message,
                  std::vector<ImVec2>& plot_points,
                  std::optional<double> arg1 = std::nullopt,
                  std::optional<double> arg2 = std::nullopt);
}
