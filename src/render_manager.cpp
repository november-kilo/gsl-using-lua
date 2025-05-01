#include <GLFW/glfw3.h>
#include <render_manager.hpp>

RenderManager::RenderManager(AppState &state, LuaManager &luaManager,
							 IntegrationManager &integrationManager)
	: state(state), luaManager(luaManager),
	  integrationManager(integrationManager) {}

void RenderManager::setRefreshCallback(std::function<void()> callback) {
	refreshCallback = std::move(callback);
}

void RenderManager::renderGui() {
	ImGui::Begin("Integration Settings");
	renderActionButtons();
	renderLuaScriptCombo();
	renderLuaFunctionName();
	renderIntegrationMethodSelector();
	renderIntegrateButton();
	renderShowPlotCheckbox();
	renderAnimationControls();
	renderIntegrationResult();
	renderErrorMessage();
	ImGui::End();

	renderPlotIfNeeded();
}

void RenderManager::renderActionButtons() {
	ImGui::SameLine(ImGui::GetWindowWidth() - 120);

	if (ImGui::Button("Refresh") && refreshCallback) {
		refreshCallback();
	}

	ImGui::SameLine();
	if (ImGui::Button("Quit")) {
		glfwSetWindowShouldClose(state.window, GLFW_TRUE);
	}
}

void RenderManager::renderLuaScriptCombo() {
	const auto &scripts = luaManager.getScripts();
	if (scripts.empty())
		return;

	if (ImGui::BeginCombo(
			"Script",
			scripts[state.selectedScriptIndex].display_name.c_str())) {
		auto availableFunctions = luaManager.getAvailableFunctions();

		for (size_t i = 0; i < scripts.size(); i++) {
			bool is_selected = (state.selectedScriptIndex == i);

			if (ImGui::Selectable(scripts[i].display_name.c_str(),
								  is_selected)) {
				if (state.selectedScriptIndex != i) {
					state.selectedScriptIndex = i;
					std::string error;
					if (luaManager.loadScript(i, state.alpha, error)) {
						availableFunctions = luaManager.getAvailableFunctions();
						state.selectedFunctionName =
							availableFunctions.empty() ? ""
													   : availableFunctions[0];
					} else {
						state.currentScriptError = std::move(error);
						ErrorManager::getInstance().addError(
							state.currentScriptError);
					}
				}
			}

			if (is_selected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
}

void RenderManager::renderLuaFunctionName() {
	auto availableFunctions = luaManager.getAvailableFunctions();
	if (availableFunctions.empty()) {
		static char selectedFunctionName_buf[128];
		strncpy(selectedFunctionName_buf, state.selectedFunctionName.c_str(),
				sizeof(selectedFunctionName_buf) - 1);
		selectedFunctionName_buf[sizeof(selectedFunctionName_buf) - 1] = '\0';

		if (ImGui::InputText("Function Name", selectedFunctionName_buf,
							 sizeof(selectedFunctionName_buf))) {
			state.selectedFunctionName = selectedFunctionName_buf;
		}
		ImGui::TextDisabled("(No functions found in script)");
	} else {
		int current_item = 0;
		for (size_t i = 0; i < availableFunctions.size(); i++) {
			if (availableFunctions[i] == state.selectedFunctionName) {
				current_item = static_cast<int>(i);
				break;
			}
		}

		if (ImGui::Combo(
				"Function Name", &current_item,
				[](void *data, int idx, const char **out_text) -> bool {
					auto *items = static_cast<std::vector<std::string> *>(data);
					if (idx < 0 || idx >= static_cast<int>(items->size())) {
						return false;
					}
					*out_text = (*items)[idx].c_str();
					return true;
				},
				&availableFunctions,
				static_cast<int>(availableFunctions.size()))) {
			state.selectedFunctionName = availableFunctions[current_item];
		}
	}
}

void RenderManager::renderIntegrationMethodSelector() {
	const char *method_names[] = {
		"QAGS - Finite limits [a,b]", "QAGI - Infinite range (-inf,+inf)",
		"QAGIU - Semi-infinite [a,+inf)", "QAGIL - Semi-infinite (-inf,b]",
		"QAG - Finite limits with key"};

	int current_method = static_cast<int>(state.selectedIntegrationMethod);
	if (ImGui::Combo("Integration Method", &current_method, method_names,
					 IM_ARRAYSIZE(method_names))) {
		state.selectedIntegrationMethod =
			static_cast<GslIntegrationMethod>(current_method);
	}

	switch (state.selectedIntegrationMethod) {
	case GslIntegrationMethod::QAGS:
	case GslIntegrationMethod::QAG:
		ImGui::InputDouble("Lower limit", &state.x_min);
		ImGui::InputDouble("Upper limit", &state.x_max);
		if (state.selectedIntegrationMethod == GslIntegrationMethod::QAG) {
			ImGui::InputInt("Key (1-6)", &state.qag_key, 1);
			fixQagKey();
		}
		break;
	case GslIntegrationMethod::QAGI:
		ImGui::TextDisabled("Integration from -inf to +inf");
		break;
	case GslIntegrationMethod::QAGIU:
		ImGui::InputDouble("Lower limit", &state.x_min);
		ImGui::TextDisabled("Upper limit: +inf");
		break;
	case GslIntegrationMethod::QAGIL:
		ImGui::TextDisabled("Lower limit: -inf");
		ImGui::InputDouble("Upper limit", &state.x_max);
		break;
	}
}

void RenderManager::renderIntegrateButton() {
	if (ImGui::Button("Integrate")) {
		fixQagKey();
		state.lastIntegrationResult =
			integrationManager.performIntegration(state, luaManager);
	}
}

void RenderManager::renderShowPlotCheckbox() {
	ImGui::SameLine();
	ImGui::Checkbox("Show Plot", &state.show_plot);
}

void RenderManager::renderAnimationControls() {
	if (ImGui::Button(state.isAnimating ? "Stop" : "Play")) {
		state.isAnimating = !state.isAnimating;

		if (state.isAnimating) {
			state.animationProgress = 0.0f;
		}
	}
	ImGui::SameLine();
	ImGui::SliderFloat("Speed", &state.animationSpeed, 0.1f, 5.0f);
}

void RenderManager::renderIntegrationResult() const {
	ImGui::Text("Result: %.6f", state.lastIntegrationResult.result);
	if (state.lastIntegrationResult.error > 0) {
		ImGui::SameLine();
		ImGui::Text("(Error: %.6f)", state.lastIntegrationResult.error);
	}
}

void RenderManager::renderErrorMessage() const {
	if (!state.currentScriptError.empty()) {
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "Error: %s",
						   state.currentScriptError.c_str());
		ErrorManager::getInstance().addError(state.currentScriptError);
	}
}

void RenderManager::renderPlotIfNeeded() {
	if (state.show_plot && !state.lastIntegrationResult.x_points.empty()) {
		if (ImGui::Begin("Function Plot", &state.show_plot)) {
			renderAxesLimits();
			renderPlot();
		}
		ImGui::End();
	}
}

void RenderManager::renderAxesLimits() {
	double y_min = std::numeric_limits<double>::max();
	double y_max = std::numeric_limits<double>::lowest();

	for (double y : state.lastIntegrationResult.extended_y_points) {
		if (!std::isnan(y) && !std::isinf(y)) {
			y_min = std::min(y_min, y);
			y_max = std::max(y_max, y);
		}
	}

	double y_margin = (y_max - y_min) * 0.1;
	y_min -= y_margin;
	y_max += y_margin;

	double x_margin = (state.x_max - state.x_min) * 0.2;
	double x_min = state.x_min - x_margin;
	double x_max = state.x_max + x_margin;

	ImPlot::SetNextAxesLimits(x_min, x_max, y_min, y_max, ImGuiCond_Always);
}

void RenderManager::renderPlotBoundLine(const char *label, double x_pos,
										const ImVec4 &color) {
	ImPlot::PushStyleColor(ImPlotCol_Line, color);
	ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 1.0f);

	double y_min = ImPlot::GetPlotLimits().Y.Min;
	double y_max = ImPlot::GetPlotLimits().Y.Max;
	double x_vals[] = {x_pos, x_pos};
	double y_vals[] = {y_min, y_max};

	ImPlot::PlotLine(label, x_vals, y_vals, 2);

	ImPlot::PopStyleVar();
	ImPlot::PopStyleColor();
}

void RenderManager::renderPlotBoundLines() {
	if (state.selectedIntegrationMethod == GslIntegrationMethod::QAGS ||
		state.selectedIntegrationMethod == GslIntegrationMethod::QAG) {

		auto pointsToShow =
			static_cast<std::vector<double>::size_type>(std::floor(
				static_cast<float>(
					state.lastIntegrationResult.extended_x_points.size()) *
				state.animationProgress));

		const double *x_points =
			state.lastIntegrationResult.extended_x_points.data();

		if (pointsToShow > 0 && x_points[pointsToShow - 1] >= state.x_min) {
			renderPlotBoundLine("Lower bound", state.x_min,
								ImVec4(1.0f, 0.2f, 0.2f, 0.8f));
		}

		if (pointsToShow > 0 && x_points[pointsToShow - 1] >= state.x_max) {
			renderPlotBoundLine("Upper bound", state.x_max,
								ImVec4(0.2f, 0.8f, 0.2f, 0.8f));
		}
	}
}

void RenderManager::calculatePointsToShow() {
	auto totalPoints = state.lastIntegrationResult.x_points.size();
	auto pointsToShow = static_cast<std::vector<double>::size_type>(
		std::floor(static_cast<float>(totalPoints) * state.animationProgress));

	if (state.isAnimating) {
		state.animationProgress +=
			ImGui::GetIO().DeltaTime * state.animationSpeed;
		if (state.animationProgress >= 1.0f) {
			state.animationProgress = 1.0f;
			state.isAnimating = false;
		}
	}

	if (!state.isAnimating) {
		pointsToShow = totalPoints;
	}

	state.pointsToShow = static_cast<int>(pointsToShow);
}

void RenderManager::renderFunctionPlot() {
	calculatePointsToShow();
	ImGui::Text("Points to show: %d", static_cast<int>(state.pointsToShow));
	ImGui::Text("Is animating: %s", state.isAnimating ? "Yes" : "No");
	ImPlot::PlotLine("f(x)",
					 state.lastIntegrationResult.extended_x_points.data(),
					 state.lastIntegrationResult.extended_y_points.data(),
					 static_cast<int>(state.pointsToShow));
}

void RenderManager::renderSignedArea() {
	if (!state.isAnimating) {
		ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);
		ImPlot::PlotShaded(
			"Signed Area", state.lastIntegrationResult.x_points.data(),
			state.lastIntegrationResult.y_points.data(),
			static_cast<int>(state.lastIntegrationResult.x_points.size()), 0.0);
		ImPlot::PopStyleVar();
	}
}

void RenderManager::renderPlot() {
	if (ImPlot::BeginPlot("Function")) {
		renderFunctionPlot();
		renderSignedArea();
		renderPlotBoundLines();
		ImPlot::EndPlot();
	}
}

void RenderManager::fixQagKey() {
	if (state.qag_key < 1) {
		state.qag_key = 6;
	}

	if (state.qag_key > 6) {
		state.qag_key = 1;
	}
}
