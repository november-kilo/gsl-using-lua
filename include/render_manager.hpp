#pragma once

#pragma once

#include <app_state.h>
#include <functional>
#include <imgui.h>
#include <implot.h>
#include <integration_manager.hpp>
#include <lua_manager.hpp>

class RenderManager {
  public:
	RenderManager(AppState &state, LuaManager &luaManager, IntegrationManager &integrationManager);

	void setRefreshCallback(std::function<void()> callback);

	void renderGui();

  private:
	AppState &state;
	LuaManager &luaManager;
	IntegrationManager &integrationManager;
	std::function<void()> refreshCallback;

	void renderActionButtons();

	void renderLuaScriptCombo();

	void renderLuaFunctionName();

	void renderIntegrationMethodSelector();

	void renderIntegrateButton();

	void renderShowPlotCheckbox();

	void calculatePointsToShow();

	void renderAnimationControls();

	void renderPlotIfNeeded();

	void renderIntegrationResult() const;

	void renderErrorMessage() const;

	void renderPlot();

	void renderAxesLimits();

	void renderFunctionPlot();

	void renderSignedArea();

	void renderPlotBoundLines();

	static void renderPlotBoundLine(const char *label, double x_pos, const ImVec4 &color);

	void fixQagKey();
};
