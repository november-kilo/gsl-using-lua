#pragma once

#include <error_manager.hpp>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_integration.h>
#include <gsl_function_params.h>
#include <integration_types.h>
#include <lua_manager.hpp>
#include <optional>
#include <sol_include.h>
#include <sstream>

class IntegrationManager {
  private:
	static std::string gsl_error_message;
	static std::string gsl_integration_error;

	static double gslFunctionWrapper(double x, void *params);
	static void handlePlotGenerationError(AppState &state,
										  IntegrationResult &result);
	static void setCurrentScriptError(AppState &state,
									  IntegrationResult &result,
									  std::string &error_message);

	bool handleIntegrationStatus(int status, AppState &state,
								 IntegrationResult &result, double abserr);
	void calculatePlotRange(const AppState &state, double &plot_x_min,
							double &plot_x_max);
	bool performGslIntegration(AppState &state, IntegrationResult &result,
							   const GslFunctionParams &params);
	std::optional<GslFunctionParams> getFunctionParams(AppState &state,
													   LuaManager &luaManager);
	bool generatePlotPoints(AppState &state, IntegrationResult &result,
							const GslFunctionParams &params);
	void initializePlotVectors(IntegrationResult &result, int num_points);
	bool calculateExtendedPoints(double plot_x_min, double plot_x_max,
								 IntegrationResult &result,
								 const GslFunctionParams &params);
	bool calculateIntegrationPoints(const AppState &state,
									IntegrationResult &result,
									const GslFunctionParams &params);
	bool calculatePoints(double start_x, double end_x,
						 std::vector<double> &x_points,
						 std::vector<double> &y_points,
						 const GslFunctionParams &params);

  public:
	IntegrationResult performIntegration(AppState &state,
										 LuaManager &luaManager);
};
