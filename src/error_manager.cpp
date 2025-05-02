#include <error_manager.hpp>
#include <imgui.h>
#include <iostream>
#include <sstream>

void ErrorManager::addError(const std::string &message) {
	errors.push_back({message, std::chrono::steady_clock::now()});
	showErrorWindow = true;
	std::cerr << message << std::endl;
}

void ErrorManager::renderErrorWindow() {
	if (!showErrorWindow) {
		return;
	}

	static const ImVec2 DEFAULT_WINDOW_SIZE(500, 300);

	ImGui::SetNextWindowSize(DEFAULT_WINDOW_SIZE, ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Error Log", &showErrorWindow)) {
		renderClearButton();
		renderErrorEntries();
	}
	ImGui::End();
}

void ErrorManager::renderClearButton() {
	if (ImGui::Button("Clear")) {
		errors.clear();
	}
	ImGui::Separator();
}

void ErrorManager::renderErrorEntries() {
	for (const auto &error : errors) {
		renderTimestamp(error.timestamp);
		ImGui::SameLine();
		ImGui::TextWrapped("%s", error.message.c_str());
		ImGui::Separator();
	}
}

void ErrorManager::renderTimestamp(const std::chrono::steady_clock::time_point &timestamp) {
	static const ImVec4 TIMESTAMP_COLOR(0.5f, 0.5f, 0.5f, 1.0f);
	std::string timeAgo = formatTimeAgo(timestamp);
	ImGui::TextColored(TIMESTAMP_COLOR, "[%s]", timeAgo.c_str());
}

std::string ErrorManager::formatTimeAgo(const std::chrono::steady_clock::time_point &timestamp) {
	static const int SECONDS_PER_MINUTE = 60;
	static const int SECONDS_PER_HOUR = 3600;

	auto now = std::chrono::steady_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - timestamp).count();

	if (elapsed < SECONDS_PER_MINUTE) {
		return std::to_string(elapsed) + " seconds ago";
	} else if (elapsed < SECONDS_PER_HOUR) {
		return std::to_string(elapsed / SECONDS_PER_MINUTE) + " minutes ago";
	} else {
		return std::to_string(elapsed / SECONDS_PER_HOUR) + " hours ago";
	}
}

void ErrorManager::gslErrorHandler(const char *reason, const char *file, int line, int gsl_errno) {
	std::stringstream ss;
	ss << "GSL Error: " << reason << " at " << file << ":" << line << " (GSL errno " << gsl_errno << ")";

	ErrorManager::getInstance().addError(ss.str());
}
