#pragma once

#include <chrono>
#include <imgui.h>
#include <string>
#include <vector>

class ErrorManager {
  public:
	struct ErrorMessage {
		std::string message;
		std::chrono::steady_clock::time_point timestamp;
	};

	static ErrorManager &getInstance() {
		static ErrorManager instance;
		return instance;
	}

	void addError(const std::string &message);
	void renderErrorWindow();
	static void gslErrorHandler(const char *reason, const char *file, int line,
								int gsl_errno);

  private:
	ErrorManager() = default;
	std::vector<ErrorMessage> errors;
	bool showErrorWindow = false;

	void renderClearButton();
	void renderErrorEntries();
	void
	renderTimestamp(const std::chrono::steady_clock::time_point &timestamp);
	std::string
	formatTimeAgo(const std::chrono::steady_clock::time_point &timestamp);
};
