#include "gsl_util.h"
#include "imgui.h"

double f(double x, void *lua_function_ref) {
    sol::reference& func_ref = *static_cast<sol::reference *>(lua_function_ref);
    sol::protected_function func(func_ref);
    return func(x);
}

int gsl_util::integrate(const std::filesystem::path& path,
                        const std::string& function_name,
                        IntegrationAlgorithm algorithm,
                        double* integration_result,
                        double* absolute_error,
                        std::string& error_message,
                        std::vector<ImVec2>& plot_points,
                        std::optional<double> arg1,
                        std::optional<double> arg2) {
    LuaScript script(path);
    if (!script.is_script_loaded()) {
        error_message = script.get_error_message();
        return -1;
    }

    sol::reference lua_function_ref = script.get_function(function_name);
    int status;
    gsl_integration_workspace *integration_workspace;
    gsl_function F;

    integration_workspace = gsl_integration_workspace_alloc(LIMIT);
    F.function = &f;
    F.params = &lua_function_ref;

    switch (algorithm) {
        case IntegrationAlgorithm::qagiu:
            if (arg1.has_value()) {
                status = gsl_integration_qagiu(&F, arg1.value(), 0, EPSILON, LIMIT, integration_workspace, integration_result, absolute_error);
            } else {
                error_message = "Missing argument for QAGIU";
                status = -1;
            }
            break;
        case IntegrationAlgorithm::qagil:
            if (arg1.has_value()) {
                status = gsl_integration_qagil(&F, arg1.value(), 0, EPSILON, LIMIT, integration_workspace, integration_result, absolute_error);
            } else {
                error_message = "Missing argument for QAGIL";
                status = -1;
            }
            break;
        case IntegrationAlgorithm::qags:
            if (arg1.has_value() && arg2.has_value()) {
                status = gsl_integration_qags(&F, arg1.value(), arg2.value(), 0, EPSILON, LIMIT, integration_workspace, integration_result, absolute_error);

                const int num_samples = 1000;
                double lower_limit = arg1.value();
                double upper_limit = arg2.value();
                double interval = (upper_limit - lower_limit) / num_samples;

                sol::protected_function fn(lua_function_ref);
                for (int i = 0; i <= num_samples; ++i) {
                    double x = lower_limit + i * interval;
                    double y = fn(x);
                    plot_points.emplace_back(x, y);
                }
            } else {
                error_message = "Missing arguments for QAGS";
                status = -1;
            }
            break;
        case IntegrationAlgorithm::qagi:
            status = gsl_integration_qagi(&F, 0, EPSILON, LIMIT, integration_workspace, integration_result, absolute_error);
            break;
        default:
            error_message = "IntegrationAlgorithm is invalid";
            status = -1;
    }

    gsl_integration_workspace_free(integration_workspace);
    return status;
}
