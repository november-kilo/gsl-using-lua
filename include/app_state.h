#pragma once

#include <GLFW/glfw3.h>
#include <integration_types.h>
#include <string>

struct AppState {
	GLFWwindow *window = nullptr;
	const char *glsl_version = nullptr;
	bool show_plot = true;

	std::string currentScriptOutput;
	std::string currentScriptError;
	size_t selectedScriptIndex = 0;
	std::string selectedFunctionName;

	double alpha = 1.0;
	double x_min = 0.0;
	double x_max = 1.0;
	GslIntegrationMethod selectedIntegrationMethod = GslIntegrationMethod::QAGS;
	int qag_key = 2;
	IntegrationResult lastIntegrationResult;

	bool isAnimating = false;
	float animationSpeed = 1.0f;
	float animationProgress = 0.0f;
	int pointsToShow = 0;
};
