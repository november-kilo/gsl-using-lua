#pragma once

#include <filesystem>
#include "imgui.h"

#define ICON_FA_FOLDER u8"\uf07b"
#define ICON_FA_FILE u8"\uf15b"

class app_window {
public:
    bool draw();

    app_window();

private:
    std::filesystem::path current_working_directory;
    std::filesystem::path selected_path;

    static void draw_qags_option(double *lower_limit, double *upper_limit);
    static void draw_qagiu_option(double *lower_limit);
    static void draw_qagil_option(double *upper_limit);
    static void draw_algorithm_select(int* selected_algorithm_index, double* lower_limit, double* upper_limit);
    void draw_file_chooser();
    void draw_file_selected();
    void draw_integrate_button(int selected_algorithm_index, int& status,
                                                    double& integration_result, double& absolute_error, std::string& error_message,
                                                    std::vector<ImVec2>& plot_points, bool& show_plot, bool& show_lua_result,
                                                    double lower_limit = 0, double upper_limit = 0);
    static void draw_integration_results(int status, double integration_result,
                                  bool show_plot, const std::vector<ImVec2>& plot_points,
                                  const std::string& error_message);
    static bool draw_quit_button();
    static void draw_plot_points(const std::vector<ImVec2>& plot_points);
    static std::tuple<double, double, double, double> calculate_plot_limits(const std::vector<ImVec2>& plot_points);
    static void draw_integration_plot(double integration_result, bool show_plot, const std::vector<ImVec2> &plot_points);
    void draw_integration_results(int status, double integration_result, bool show_plot, const std::vector<ImVec2> &plot_points);
};
