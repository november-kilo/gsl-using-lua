This application is written in C++, built with [CMake](https://cmake.org/), and uses the following:

* [ImGui](https://github.com/ocornut/imgui.git) (git submodule)
* [ImPlot](https://github.com/epezent/implot.git) (git submodule)
* [Lua](https://www.lua.org/)
* [Sol2](https://github.com/ThePhD/sol2.git) (git submodule)
* [Gnu Scientific Library](https://www.gnu.org/software/gsl/)
* [FontAwesome](https://fontawesome.com/download) v6.6.0 for the desktop (free version).

This app integrates a univariate function implemented in a Lua script.
The function must be named `integration_fn` and takes one argument, for instance:
```lua
function integration_fn(x)
    return x * math.sin(4 * x)
end
```
The script must also provide an error handling function.
The function must be named `error_fn` and take one string argument, for instance:
```lua
function error_fn(msg)
    return "lua error: " .. msg
end
```

I wrote this app because it's useful to me, and to learn C++ and the libraries used.
The code, no doubt, needs more work.
