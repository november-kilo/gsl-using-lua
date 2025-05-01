This application is written in C++, built with [meson](https://mesonbuild.com/), and
uses the following:
* [ImGui](https://github.com/ocornut/imgui.git) (git submodule)
* [ImPlot](https://github.com/epezent/implot.git) (git submodule)
* [Lua](https://www.lua.org/)
* [Sol2](https://github.com/ThePhD/sol2.git)
* [Gnu Scientific Library](https://www.gnu.org/software/gsl/)

This app integrates a univariate function implemented in a Lua script.
The `scripts/` directory is searched for Lua scripts and populates a combo box.
Once a script is chosen, a function defined in the script is selected.
The function takes two inputs, the `x` and another value `alpha` (currently the
value of the later is always `1.0`). For instance:
```lua
function modulated_sine(x, alpha)
    return x * math.sin(4 * x)
end
```

Some of GSL's integration algorithms are presented in a combo,
with appropriate inputs for the limits.

The "Integrate" button performs the integration and updates the plot,
which includes a shaded signed area.

The "Play" button animates the curve from negative to positive.

The "Refresh" button rereads the scripts and resets the UI.

I wrote this app because it's useful to me, and to learn C++ and the libraries used.
The code, no doubt, needs more work.
