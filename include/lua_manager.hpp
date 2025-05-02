#pragma once
#include <filesystem>
#include <script_file.h>
#include <set>
#include <sol_include.h>
#include <string>
#include <vector>

class LuaManager {
  private:
	sol::state &lua_;
	std::vector<ScriptFile> scripts_;
	std::vector<std::string> available_functions_;

	void updateAvailableFunctions();

  public:
	explicit LuaManager(sol::state &lua);

	void loadScripts(const std::string &directory);

	[[nodiscard]]
	bool loadScript(size_t scriptIndex, double alpha, std::string &error);

	[[nodiscard]]
	const std::vector<ScriptFile> &getScripts() const;

	[[nodiscard]]
	const std::vector<std::string> &getAvailableFunctions() const;

	[[nodiscard]]
	std::optional<sol::protected_function> getFunction(const std::string &functionName);

	[[nodiscard]]
	bool hasScripts() const;
};
