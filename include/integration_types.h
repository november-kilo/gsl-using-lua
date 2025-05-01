#pragma once

#include <vector>

enum class GslIntegrationMethod {
	QAGS,  // Finite limits [a,b]
	QAGI,  // Infinite range (-∞,+∞)
	QAGIU, // Semi-infinite range [a,+∞)
	QAGIL, // Semi-infinite range (-∞,b]
	QAG	   // Finite limits [a,b] with key parameter
};

struct IntegrationResult {
	double result = 0.0;
	double error = 0.0;
	std::vector<double> x_points;
	std::vector<double> y_points;
	std::vector<double> extended_x_points;
	std::vector<double> extended_y_points;
};
