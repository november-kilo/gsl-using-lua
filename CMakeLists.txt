cmake_minimum_required(VERSION 3.24)
project(gsl_lua)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(IMGUI_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui")
set(IMPLOT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/implot")
set(SOL3_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/sol2")
set(GSL_ROOT_DIR "/usr/local/homebrew/Cellar/gsl/2.8")

set(IMGUI_SOURCES
        ${IMGUI_ROOT}/imgui.cpp
        ${IMGUI_ROOT}/imgui_draw.cpp
        ${IMGUI_ROOT}/imgui_tables.cpp
        ${IMGUI_ROOT}/imgui_widgets.cpp
        ${IMGUI_ROOT}/imgui_demo.cpp
        ${IMGUI_ROOT}/backends/imgui_impl_glfw.cpp
        ${IMGUI_ROOT}/backends/imgui_impl_opengl3.cpp
        ${IMPLOT_ROOT}/implot.cpp
        ${IMPLOT_ROOT}/implot_items.cpp
        ${IMPLOT_ROOT}/implot_demo.cpp
        src/lua_util.cpp
        src/lua_util.h
        src/gsl_util.cpp
        src/gsl_util.h
        src/file_util.cpp
        src/file_util.h
)

add_executable(gsl_lua
        src/main.cpp
        src/app_window.h
        src/app_window.cpp
        ${IMGUI_SOURCES}
)

target_include_directories(gsl_lua PRIVATE
        ${IMGUI_ROOT}
        ${IMPLOT_ROOT}
        ${GSL_ROOT_DIR}/include
        ${IMGUI_ROOT}/backends
        /usr/local/include
)

add_subdirectory(thirdparty/sol2)
include_directories(thirdparty/sol2/include)

find_package(glfw3 REQUIRED)
target_link_libraries(gsl_lua PRIVATE glfw)

find_package(OpenGL REQUIRED)
target_include_directories(gsl_lua PRIVATE ${OPENGL_INCLUDE_DIR})
target_link_libraries(gsl_lua PRIVATE ${OPENGL_gl_LIBRARY})

find_library(GSL_LIB gsl ${GSL_ROOT_DIR}/lib)
find_library(GSLCBLAS_LIB gslcblas ${GSL_ROOT_DIR}/lib)
target_link_libraries(gsl_lua PRIVATE ${GSL_LIB} ${GSLCBLAS_LIB})

find_library(LUA_LIB lua /usr/local/lib)
target_link_libraries(gsl_lua PRIVATE ${LUA_LIB})
