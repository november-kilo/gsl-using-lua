cmake_minimum_required(VERSION 3.27)
project(app C)
set(CMAKE_C_STANDARD 11)
include_directories(/usr/local/include)
link_directories(/usr/local/lib)
add_executable(app
        src/main.c src/lua_evaluate.c src/lua_integrate.c src/main_loop.c src/main_loop.h
)
find_package(GSL REQUIRED)
target_link_libraries(app readline m lua gsl)
