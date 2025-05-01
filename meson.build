project('gsl_using_lua', 'cpp',
        version : '1.0.0',
        default_options : [
            'cpp_std=c++20'
        ]
)

inc = include_directories(
    'third_party',
    'third_party/sol',
    'third_party/imgui',
    'third_party/imgui/backends',
    'third_party/implot',
    'include'
)

lua_dep = dependency('lua')
gsl_dep = dependency('gsl')
glfw_dep = dependency('glfw3')
gl_dep = dependency('gl')

class_src = [
    'src/application.cpp',
    'src/error_manager.cpp',
    'src/integration_manager.cpp',
    'src/lua_manager.cpp',
    'src/render_manager.cpp'
]

imgui_src = [
    'third_party/imgui/imgui.cpp',
    'third_party/imgui/imgui_demo.cpp',
    'third_party/imgui/imgui_draw.cpp',
    'third_party/imgui/imgui_tables.cpp',
    'third_party/imgui/imgui_widgets.cpp',
    'third_party/imgui/backends/imgui_impl_glfw.cpp',
    'third_party/imgui/backends/imgui_impl_opengl3.cpp',
    'third_party/implot/implot.cpp',
    'third_party/implot/implot_items.cpp'
]

gsl_using_lua = executable('gsl_using_lua',
                        ['src/main.cpp'] + class_src + imgui_src,
                        include_directories : inc,
                        dependencies : [ glfw_dep, gl_dep, lua_dep, gsl_dep ],
                        cpp_args : ['-stdlib=libc++'],
                        link_args : ['-stdlib=libc++']
)
