#include <application.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <implot.h>
#include <iostream>

void Application::glfwErrorCallback(int error, const char *description) {
	std::stringstream ss;
	ss << "GLFW Error " << error << ": " << description << std::endl;
	ErrorManager::getInstance().addError(ss.str());
}

Application::Application()
	: state{}, lua{}, luaManager(lua), integrationManager{} {
	lua.open_libraries(sol::lib::base, sol::lib::math);
	glfwSetErrorCallback(glfwErrorCallback);
}

Application::~Application() { shutdown(); }

void Application::init() {
	initGlfw();
	initImGui();
	initApp();

	renderManager =
		std::make_unique<RenderManager>(state, luaManager, integrationManager);
	renderManager->setRefreshCallback([this]() { refresh(); });
}

void Application::initGlfw() {
	if (!glfwInit()) {
		throw std::runtime_error("Failed to initialize GLFW");
	}

#if defined(__APPLE__)
	state.glsl_version = "#version 150";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
	state.glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

	state.window =
		glfwCreateWindow(1280, 720, "Integration GUI", nullptr, nullptr);
	if (!state.window) {
		glfwTerminate();
		throw std::runtime_error("Failed to create GLFW window");
	}

	glfwMakeContextCurrent(state.window);
	glfwSwapInterval(1);
}

void Application::initImGui() const {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImPlot::CreateContext();

	ImGuiIO &io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.FontGlobalScale = 1.0f;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(state.window, true);
	ImGui_ImplOpenGL3_Init(state.glsl_version);
}

void Application::initLua() {
	luaManager.loadScripts("scripts");
	if (luaManager.hasScripts()) {
		std::string error;
		if (luaManager.loadScript(0, state.alpha, error)) {
			if (!luaManager.getAvailableFunctions().empty()) {
				state.selectedFunctionName =
					luaManager.getAvailableFunctions()[0];
			}
		} else {
			state.currentScriptError = std::move(error);
			ErrorManager::getInstance().addError(state.currentScriptError);
		}
	}
}

void Application::initApp() {
	gsl_set_error_handler(&ErrorManager::gslErrorHandler);
	initLua();
}

void Application::refreshState() {
	state.currentScriptError.clear();
	state.currentScriptOutput.clear();
	state.lastIntegrationResult = IntegrationResult{};
}

void Application::refreshLua() {
	luaManager.loadScripts("scripts");
	if (luaManager.hasScripts()) {
		std::string error;
		if (luaManager.loadScript(state.selectedScriptIndex, state.alpha,
								  error)) {
			if (!luaManager.getAvailableFunctions().empty()) {
				state.selectedFunctionName =
					luaManager.getAvailableFunctions()[0];
			}
		} else {
			state.currentScriptError = std::move(error);
			ErrorManager::getInstance().addError(state.currentScriptError);
		}
	}
}

void Application::refresh() {
	refreshState();
	refreshLua();
}

void Application::pre_render() {
	glfwPollEvents();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void Application::post_render() const {
	ImGui::Render();

	int display_w, display_h;
	glfwGetFramebufferSize(state.window, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);

	glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
	glClear(GL_COLOR_BUFFER_BIT);

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(state.window);
}

void Application::render_loop() {
	while (!glfwWindowShouldClose(state.window)) {
		pre_render();
		renderManager->renderGui();
		ErrorManager::getInstance().renderErrorWindow();
		post_render();
	}
}

void Application::shutdown() {
	renderManager.reset();

	ImPlot::DestroyContext();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	if (state.window) {
		glfwDestroyWindow(state.window);
		state.window = nullptr;
	}

	glfwTerminate();
}

void Application::run() {
	try {
		init();
		render_loop();
	} catch (const std::exception &e) {
		std::cerr << "Error: " << e.what() << std::endl;
		shutdown();
		throw;
	}
}
