#pragma once

#include <filesystem>
#include <string>
#include <sol/sol.hpp>

#define SOL_ALL_SAFETIES_ON 1
#define SOL_EXCEPTIONS_ALWAYS_UNSAFE 1

class LuaScript {
    bool script_loaded;
    std::string error_message;
    sol::state lua;

public:
    explicit LuaScript(const std::filesystem::path& script_path);

    [[nodiscard]] bool is_script_loaded() const;
    [[nodiscard]] std::string get_error_message() const;
    [[nodiscard]] sol::protected_function get_function(const std::string& function_name) const ;

    template<typename... Args>
    auto call(const std::string& function, Args... args) {
        sol::protected_function func = lua[function];
        return func(std::forward<Args>(args)...);
    }
};
