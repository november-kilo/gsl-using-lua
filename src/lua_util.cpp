#include "lua_util.h"
#include <sol/sol.hpp>

LuaScript::LuaScript(const std::filesystem::path &script_path) {
    lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math);

    try {
        lua.script_file(script_path.string());
        script_loaded = true;
        error_message = "";
    } catch (const sol::error& e) {
        script_loaded = false;
        error_message = e.what();
    }

    if (script_loaded) {
        sol::reference lua_function_ref = lua["integration_fn"];
        sol::protected_function func(lua_function_ref);
        func.set_error_handler(lua["error_fn"]);
        auto result = func(1);
        if (!result.valid()) {
            sol::error err = result;
            std::string what = err.what();
            script_loaded = false;
            error_message = what;
        }
    }
}

bool LuaScript::is_script_loaded() const {
    return script_loaded;
}

std::string LuaScript::get_error_message() const {
    return error_message;
}

sol::protected_function LuaScript::get_function(const std::string &function_name) const {
    return lua[function_name];
}
