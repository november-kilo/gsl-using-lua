#include <app_state.h>
#include <integration_manager.hpp>

std::string IntegrationManager::gsl_error_message;
std::string IntegrationManager::gsl_integration_error;

IntegrationResult IntegrationManager::performIntegration(AppState &state, LuaManager &luaManager) {
	IntegrationResult result;
	state.currentScriptError.clear();

	try {
		auto params = getFunctionParams(state, luaManager);
		if (!params) {
			return result;
		}

		if (!performGslIntegration(state, result, params.value())) {
			return result;
		}

		printf("result = % .18f\n", result.result);

		if (!generatePlotPoints(state, result, params.value())) {
			handlePlotGenerationError(state, result);
		}

	} catch (const sol::error &e) {
		std::string error_message = std::string("Lua error: ") + e.what();
		setCurrentScriptError(state, result, error_message);
	} catch (const std::exception &e) {
		std::string error_message = std::string("Error: ") + e.what();
		setCurrentScriptError(state, result, error_message);
	} catch (...) {
		std::string error_message = "Unknown error occurred";
		setCurrentScriptError(state, result, error_message);
	}

	return result;
}

std::optional<GslFunctionParams> IntegrationManager::getFunctionParams(AppState &state, LuaManager &luaManager) {
	auto func = luaManager.getFunction(state.selectedFunctionName);
	if (!func.has_value()) {
		state.currentScriptError = "Could not find valid function '" + state.selectedFunctionName + "'";
		return std::nullopt;
	}

	return GslFunctionParams{func.value(), state.alpha};
}

bool IntegrationManager::performGslIntegration(AppState &state, IntegrationResult &result,
											   const GslFunctionParams &params) {
	gsl_integration_workspace *workspace = gsl_integration_workspace_alloc(5000);
	gsl_integration_error.clear();

	gsl_function F;
	F.function = &IntegrationManager::gslFunctionWrapper;
	F.params = const_cast<GslFunctionParams *>(&params);

	double abserr;
	int status;

	switch (state.selectedIntegrationMethod) {
	case GslIntegrationMethod::QAGS:
		status =
			gsl_integration_qags(&F, state.x_min, state.x_max, 1e-7, 1e-7, 1000, workspace, &result.result, &abserr);
		break;

	case GslIntegrationMethod::QAGI:
		status = gsl_integration_qagi(&F, 1e-7, 1e-7, 1000, workspace, &result.result, &abserr);
		break;

	case GslIntegrationMethod::QAGIU:
		status = gsl_integration_qagiu(&F, state.x_min, 1e-7, 1e-7, 1000, workspace, &result.result, &abserr);
		break;

	case GslIntegrationMethod::QAGIL:
		status = gsl_integration_qagil(&F, state.x_max, 1e-7, 1e-7, 1000, workspace, &result.result, &abserr);
		break;

	case GslIntegrationMethod::QAG:
		status = gsl_integration_qag(&F, state.x_min, state.x_max, 1e-7, 1e-7, 1000, state.qag_key, workspace,
									 &result.result, &abserr);
		break;
	}

	bool success = handleIntegrationStatus(status, state, result, abserr);
	gsl_integration_workspace_free(workspace);
	return success;
}

bool IntegrationManager::handleIntegrationStatus(int status, AppState &state, IntegrationResult &result,
												 double abserr) {
	if (status != GSL_SUCCESS) {
		if (!gsl_integration_error.empty()) {
			state.currentScriptError = gsl_integration_error;
		} else {
			state.currentScriptError = gsl_error_message;
		}

		result.result = 0.0;
		result.error = 0.0;

		return false;
	}

	if (!gsl_integration_error.empty()) {
		state.currentScriptError = gsl_integration_error;
		return false;
	}

	result.error = abserr;
	return true;
}

bool IntegrationManager::generatePlotPoints(AppState &state, IntegrationResult &result,
											const GslFunctionParams &params) {
	double plot_x_min, plot_x_max;
	calculatePlotRange(state, plot_x_min, plot_x_max);

	const int num_points = 200;
	initializePlotVectors(result, num_points);

	if (!calculateExtendedPoints(plot_x_min, plot_x_max, result, params)) {
		return false;
	}

	return calculateIntegrationPoints(state, result, params);
}

void IntegrationManager::initializePlotVectors(IntegrationResult &result, int num_points) {
	result.extended_x_points.resize(num_points);
	result.extended_y_points.resize(num_points);
	result.x_points.resize(num_points);
	result.y_points.resize(num_points);
}

bool IntegrationManager::calculateExtendedPoints(double plot_x_min, double plot_x_max, IntegrationResult &result,
												 const GslFunctionParams &params) {
	for (size_t i = 0; i < result.extended_x_points.size(); i++) {
		try {
			double x = plot_x_min + (plot_x_max - plot_x_min) * i / (result.extended_x_points.size() - 1.0);
			result.extended_x_points[i] = x;
			result.extended_y_points[i] = gslFunctionWrapper(x, const_cast<GslFunctionParams *>(&params));

			if (std::isnan(result.extended_y_points[i]) || std::isinf(result.extended_y_points[i])) {
				return false;
			}
		} catch (...) {
			return false;
		}
	}
	return true;
}

bool IntegrationManager::calculateIntegrationPoints(const AppState &state, IntegrationResult &result,
													const GslFunctionParams &params) {
	switch (state.selectedIntegrationMethod) {
	case GslIntegrationMethod::QAGS:
	case GslIntegrationMethod::QAG:
		return calculatePoints(state.x_min, state.x_max, result.x_points, result.y_points, params);

	case GslIntegrationMethod::QAGI:
		result.x_points = result.extended_x_points;
		result.y_points = result.extended_y_points;
		return true;

	case GslIntegrationMethod::QAGIU:
		return calculatePoints(state.x_min, state.x_min + 10.0, result.x_points, result.y_points, params);

	case GslIntegrationMethod::QAGIL:
		return calculatePoints(state.x_max - 10.0, state.x_max, result.x_points, result.y_points, params);
	}
	return true;
}

bool IntegrationManager::calculatePoints(double start_x, double end_x, std::vector<double> &x_points,
										 std::vector<double> &y_points, const GslFunctionParams &params) {
	for (size_t i = 0; i < x_points.size(); i++) {
		try {
			double x = start_x + (end_x - start_x) * i / (x_points.size() - 1.0);
			x_points[i] = x;
			y_points[i] = gslFunctionWrapper(x, const_cast<GslFunctionParams *>(&params));
		} catch (...) {
			return false;
		}
	}
	return true;
}

void IntegrationManager::calculatePlotRange(const AppState &state, double &plot_x_min, double &plot_x_max) {
	switch (state.selectedIntegrationMethod) {
	case GslIntegrationMethod::QAGS:
	case GslIntegrationMethod::QAG:
		plot_x_min = state.x_min - (state.x_max - state.x_min) * 0.2;
		plot_x_max = state.x_max + (state.x_max - state.x_min) * 0.2;
		break;
	case GslIntegrationMethod::QAGI:
		plot_x_min = -10.0;
		plot_x_max = 10.0;
		break;
	case GslIntegrationMethod::QAGIU:
		plot_x_min = state.x_min;
		plot_x_max = state.x_min + 10.0;
		break;
	case GslIntegrationMethod::QAGIL:
		plot_x_min = state.x_max - 10.0;
		plot_x_max = state.x_max;
		break;
	}
}

double IntegrationManager::gslFunctionWrapper(double x, void *params) {
	auto *p = static_cast<GslFunctionParams *>(params);

	try {
		auto result = p->func(x, p->alpha);
		if (!result.valid()) {
			gsl_integration_error = "Lua function returned invalid result";
			return 0.0;
		}
		return result.get<double>();
	} catch (const std::exception &e) {
		gsl_integration_error = std::string("Error in function evaluation: ") + e.what();
		return 0.0;
	}
}

void IntegrationManager::handlePlotGenerationError(AppState &state, IntegrationResult &result) {
	state.currentScriptError += "\nWarning: Some points could not be plotted due to invalid values";
	result.x_points.clear();
	result.y_points.clear();
	result.extended_x_points.clear();
	result.extended_y_points.clear();
}

void IntegrationManager::setCurrentScriptError(AppState &state, IntegrationResult &result, std::string &error_message) {
	state.currentScriptError = error_message;
	ErrorManager::getInstance().addError(state.currentScriptError);
	result.result = result.error = 0.0;
}
