#include "lua_manager.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>

LuaManager::LuaManager(sol::state &lua) : lua_(lua) {
	lua_.open_libraries(sol::lib::base, sol::lib::math);
}

bool LuaManager::hasScripts() const { return !scripts_.empty(); }

const std::vector<std::string> &LuaManager::getAvailableFunctions() const {
	return available_functions_;
}

const std::vector<ScriptFile> &LuaManager::getScripts() const {
	return scripts_;
}

void LuaManager::loadScripts(const std::string &directory) {
	scripts_.clear();
	try {
		for (const auto &entry :
			 std::filesystem::directory_iterator(directory)) {
			if (entry.path().extension() == ".lua") {
				std::ifstream file(entry.path());
				std::string content((std::istreambuf_iterator<char>(file)),
									std::istreambuf_iterator<char>());
				scripts_.push_back({entry.path().filename().string(), content});
			}
		}

		std::sort(scripts_.begin(), scripts_.end(),
				  [](const ScriptFile &a, const ScriptFile &b) {
					  return a.display_name < b.display_name;
				  });

	} catch (const std::filesystem::filesystem_error &e) {
		std::cerr << "Filesystem error: " << e.what() << std::endl;
	}
}

bool LuaManager::loadScript(size_t scriptIndex, double alpha,
							std::string &error) {
	if (scriptIndex >= scripts_.size()) {
		error = "Invalid script index";
		return false;
	}

	auto script_result = lua_.safe_script(scripts_[scriptIndex].content,
										  sol::script_pass_on_error);
	if (!script_result.valid()) {
		sol::error err = script_result;
		error = err.what();
		return false;
	}

	lua_["alpha"] = alpha;
	updateAvailableFunctions();
	return true;
}

std::optional<sol::protected_function>
LuaManager::getFunction(const std::string &functionName) {
	auto func = lua_[functionName];
	if (!func.valid()) {
		return std::nullopt;
	}
	return sol::protected_function(func);
}

void LuaManager::updateAvailableFunctions() {
	available_functions_.clear();

	static const std::set<std::string> builtin_functions = {
		"assert",		"collectgarbage",
		"dofile",		"error",
		"getmetatable", "ipairs",
		"load",			"loadfile",
		"next",			"pairs",
		"pcall",		"print",
		"rawequal",		"rawget",
		"rawlen",		"rawset",
		"require",		"select",
		"setmetatable", "tonumber",
		"tostring",		"type",
		"xpcall",		"warn",
		"abs",			"acos",
		"asin",			"atan",
		"ceil",			"cos",
		"deg",			"exp",
		"floor",		"log",
		"max",			"min",
		"pi",			"rad",
		"sin",			"sqrt",
		"tan"};

	sol::global_table global = lua_.globals();
	global.for_each([&](const sol::object &key, const sol::object &value) {
		if (value.get_type() == sol::type::function) {
			std::string name = key.as<std::string>();
			if (!name.starts_with("sol") &&
				builtin_functions.find(name) == builtin_functions.end()) {
				available_functions_.push_back(name);
			}
		}
	});

	std::sort(available_functions_.begin(), available_functions_.end());
}
