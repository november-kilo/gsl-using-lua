#include <filesystem>
#include "app_window.h"
#include "gsl_util.h"
#include <implot.h>

static std::string gsl_error_message;

void my_gsl_error_handler(const char *reason, const char *file, int line, int gsl_errno) {
    std::ostringstream msg;
    msg << "GSL error " << gsl_errno << " in file '" << file << "', at line " << line << ": " << reason;
    gsl_error_message = msg.str();
}

app_window::app_window() {
    current_working_directory = std::filesystem::current_path();
    gsl_set_error_handler(&my_gsl_error_handler);
}

bool app_window::draw() {
    constexpr static auto window_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
    constexpr static auto window_size = ImVec2(600.0F, 600.0F);
    constexpr static auto window_pos = ImVec2(0.0F, 0.0F);

    ImGui::SetNextWindowSize(window_size);
    ImGui::SetNextWindowPos(window_pos);

    ImGui::Begin("Function Integrator", nullptr, window_flags);

    if (draw_quit_button()) {
        return false;
    }

    ImGui::Separator();

    static double lower_limit = 0.0;
    static double upper_limit = 1.0;
    static int selected_algorithm_index = 0;
    static bool show_lua_result = false;
    static int status;
    static double integration_result = 0;
    static std::string error_message;
    static std::vector<ImVec2> plot_points;
    static bool show_plot = false;
    static double absolute_error;

    draw_file_chooser();
    draw_file_selected();
    draw_algorithm_select(&selected_algorithm_index, &lower_limit, &upper_limit);
    draw_integrate_button(selected_algorithm_index, status, integration_result, absolute_error, error_message, plot_points, show_plot, show_lua_result, lower_limit, upper_limit);

    if (!gsl_error_message.empty()) {
        ImGui::Text("%s", gsl_error_message.c_str());
    }

    if (show_lua_result) {
        draw_integration_results(status, integration_result, show_plot, plot_points, error_message);
    }

    ImGui::End();
    return true;
}

void app_window::draw_qags_option(double *lower_limit, double *upper_limit) {
    ImGui::InputDouble("Lower limit", lower_limit, 0.0);
    ImGui::InputDouble("Upper limit", upper_limit, 1.0);
    ImGui::Text("Integrate from %g to %g", *lower_limit, *upper_limit);
}

void app_window::draw_qagiu_option(double *lower_limit) {
    ImGui::InputDouble("Lower limit", lower_limit, 0.0);
    ImGui::Text("Integrate from %g to +inf", *lower_limit);
}

void app_window::draw_qagil_option(double *upper_limit) {
    ImGui::InputDouble("Upper limit", upper_limit, 1.0);
    ImGui::Text("Integrate from -inf to %g", 1.0);
}

void app_window::draw_file_chooser() {
    ImGui::Text("Directory: %s", current_working_directory.string().c_str());

    if (ImGui::Button("Up one")) {
        if (current_working_directory.has_parent_path()) {
            current_working_directory = current_working_directory.parent_path();
        }
    }

    for (const auto &path_entry: std::filesystem::directory_iterator(current_working_directory)) {
        const auto is_selected = path_entry.path() == selected_path;
        const auto is_directory = path_entry.is_directory();
        const auto is_file = path_entry.is_regular_file();
        auto entry_name = path_entry.path().filename().string();
        auto is_selectable = false;

        if (is_directory) {
            entry_name.insert(0, ICON_FA_FOLDER " ");
            is_selectable = true;
        } else if (is_file) {
            if (path_entry.path().extension().string() == ".lua") {
                entry_name.insert(0, ICON_FA_FILE " ");
                is_selectable = true;
            }
        }

        if (is_selectable && ImGui::Selectable(entry_name.c_str(), is_selected)) {
            if (is_directory) {
                current_working_directory /= path_entry.path().filename();
            }

            selected_path = path_entry.path();
        }
    }
}

void app_window::draw_file_selected() {
    ImGui::Separator();

    if (std::filesystem::is_regular_file(selected_path)) {
        ImGui::Text("Selected file: %s", selected_path.string().c_str());
    } else {
        ImGui::Text("Please a lua script file.");
    }

    ImGui::Separator();
}

void app_window::draw_algorithm_select(int* selected_algorithm_index, double* lower_limit, double* upper_limit) {
        ImGui::Combo("Algorithm", selected_algorithm_index, "qags\0qagi\0qagiu\0qagil\0");

        switch (*selected_algorithm_index) {
            case 0:
                draw_qags_option(lower_limit, upper_limit);
                break;
            case 1:
                ImGui::Text("Integrate from -inf to +inf");
                break;
            case 2:
                draw_qagiu_option(lower_limit);
                break;
            case 3:
                draw_qagil_option(upper_limit);
                break;
            default:
                ImGui::Text("Unknown algorithm selected.");
        }
    }

void app_window::draw_integrate_button(int selected_algorithm_index, int& status,
                                       double& integration_result, double& absolute_error, std::string& error_message,
                                       std::vector<ImVec2>& plot_points, bool& show_plot, bool& show_lua_result,
                                       double lower_limit, double upper_limit) {
    if (std::filesystem::is_regular_file(selected_path) && ImGui::Button("Integrate")) {
        show_plot = false;
        switch (selected_algorithm_index) {
            case 0:
                plot_points.clear();
                status = gsl_util::integrate(selected_path, "integration_fn", IntegrationAlgorithm::qags, &integration_result, &absolute_error, error_message, plot_points, lower_limit, upper_limit);
                show_plot = true;
                break;
            case 1:
                status = gsl_util::integrate(selected_path, "integration_fn", IntegrationAlgorithm::qagi, &integration_result, &absolute_error, error_message, plot_points);
                break;
            case 2:
                status = gsl_util::integrate(selected_path, "integration_fn", IntegrationAlgorithm::qagiu, &integration_result, &absolute_error, error_message, plot_points, lower_limit);
                break;
            case 3:
                status = gsl_util::integrate(selected_path, "integration_fn", IntegrationAlgorithm::qagil, &integration_result, &absolute_error, error_message, plot_points, upper_limit);
                break;
            default:
                break;
        }

        if (status == GSL_SUCCESS) {
            error_message = "";
        }

        show_lua_result = true;
    }
}

void app_window::draw_integration_results(int status, double integration_result, bool show_plot,
                                          const std::vector<ImVec2> &plot_points, const std::string &error_message) {
    if (status == 0) {
        draw_integration_plot(integration_result, show_plot, plot_points);
    } else {
        ImGui::Text("%s", error_message.c_str());
    }
}

void app_window::draw_integration_plot(double integration_result, bool show_plot, const std::vector<ImVec2> &plot_points) {
    ImGui::Separator();
    ImGui::Text("Answer: %g", integration_result);
    if (show_plot && !plot_points.empty()) {
        gsl_error_message = "";
        auto [x_min, x_max, y_min, y_max] = calculate_plot_limits(plot_points);
        ImPlot::SetNextAxesLimits(x_min, x_max, y_min, y_max, ImPlotCond_Always);
        if (ImPlot::BeginPlot("Area Under Curve")) {
            draw_plot_points(plot_points);
            ImPlot::EndPlot();
        }
    }
}

std::tuple<double, double, double, double> app_window::calculate_plot_limits(const std::vector<ImVec2>& plot_points) {
    double x_min = std::numeric_limits<double>::infinity();
    double x_max = -std::numeric_limits<double>::infinity();
    double y_min = std::numeric_limits<double>::infinity();
    double y_max = -std::numeric_limits<double>::infinity();
    for (const auto& point : plot_points) {
        if (point.x < x_min) { x_min = point.x; }
        if (point.x > x_max) { x_max = point.x; }
        if (point.y < y_min) { y_min = point.y; }
        if (point.y > y_max) { y_max = point.y; }
    }
    return std::make_tuple(x_min, x_max, y_min, y_max);
}

void app_window::draw_plot_points(const std::vector<ImVec2>& plot_points) {
    if (plot_points.size() <= std::numeric_limits<int>::max()) {
        std::vector<float> x(plot_points.size());
        std::vector<float> y(plot_points.size());
        std::vector<float> zero(plot_points.size(), 0.f);  // Array of zero values for shading
        for(size_t i = 0; i < plot_points.size(); i++) {
            x[i] = plot_points[i].x;
            y[i] = plot_points[i].y;
        }
        ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        ImPlot::PushStyleColor(ImPlotCol_Fill, ImVec4(1.0f, 0.5f, 0.0f, 0.33f));
        ImPlot::PlotLine("f(x)", x.data(), y.data(), static_cast<int>(plot_points.size()));
        ImPlot::PlotShaded("area", x.data(), zero.data(), y.data(), static_cast<int>(plot_points.size()));
        ImPlot::PopStyleColor(2);
    }
    else {
        ImGui::Text("Too many points to plot.");
    }
}

bool app_window::draw_quit_button() {
    ImGui::Separator();
    if (ImGui::Button("Quit")) {
        ImGui::End();
        return true;
    }

    return false;
}
