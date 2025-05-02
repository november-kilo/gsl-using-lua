#pragma once

#define GL_SILENCE_DEPRECATION
#include <app_state.h>
#include <error_manager.hpp>
#include <integration_manager.hpp>
#include <lua_manager.hpp>
#include <memory>
#include <render_manager.hpp>
#include <sstream>
#include <stdexcept>

class Application {
  public:
	Application();
	~Application();
	void run();

  private:
	AppState state;
	LuaManager luaManager;
	IntegrationManager integrationManager;
	std::unique_ptr<RenderManager> renderManager;

	void init();
	void initGlfw();
	void initImGui() const;
	void initApp();
	void initLua();
	void refresh();
	void refreshState();
	void refreshLua();
	void shutdown();
	void render_loop();
	static void pre_render();
	void post_render() const;
	static void glfwErrorCallback(int error, const char *description);
	static void gslErrorHandler(const char *reason, const char *file, int line, int gsl_errno);
};
