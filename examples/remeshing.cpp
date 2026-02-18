// Copyright 2011-2019 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#include <pmp/viewers/mesh_viewer.h>
#include <pmp/algorithms/features.h>
#include <pmp/algorithms/remeshing.h>
#include <pmp/algorithms/utilities.h>

#include <imgui.h>

using namespace pmp;

class Viewer : public MeshViewer
{
public:
    Viewer(const char* title, int width, int height);

protected:
    void process_imgui() override;
};

Viewer::Viewer(const char* title, int width, int height)
    : MeshViewer(title, width, height)
{
    set_draw_mode("Hidden Line");
    crease_angle_ = 0.0;
}

void Viewer::process_imgui()
{
    MeshViewer::process_imgui();

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Features", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Spacing();

        static int feature_angle = 70;
        ImGui::PushItemWidth(80);
        ImGui::SliderInt("##feature_angle", &feature_angle, 1, 180);
        ImGui::PopItemWidth();
        ImGui::SameLine();
        if (ImGui::Button("Detect Features"))
        {
            clear_features(mesh_);
            detect_features(mesh_, feature_angle);
            update_mesh();
        }
    }

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Uniform Remeshing",
                                ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Spacing();

        ImGui::PushItemWidth(80);

        static double edge_length{0.01};
        ImGui::InputDouble("Edge Length", &edge_length, 0, 0, "%g");
        ImGui::SameLine();
        if (ImGui::Button("Mean"))
        {
            edge_length = mean_edge_length(mesh_);
        }

        static int n_iterations{10};
        ImGui::SliderInt("Iterations##uniform", &n_iterations, 1, 20);

        static bool use_projection{true};
        ImGui::Checkbox("Use Projection##uniform", &use_projection);

        static bool scale_lengths{true};
        ImGui::Checkbox("Scale Lengths##uniform", &scale_lengths);

        ImGui::Spacing();

        if (ImGui::Button("Remesh##uniform"))
        {
            try
            {
                auto scaling = scale_lengths ? bounds(mesh_).size() : 1.0;
                uniform_remeshing(mesh_, edge_length * scaling, n_iterations,
                                  use_projection);
            }
            catch (const InvalidInputException& e)
            {
                std::cerr << e.what() << std::endl;
                return;
            }
            update_mesh();
        }

        ImGui::PopItemWidth();
    }

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Adaptive Remeshing",
                                ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Spacing();

        ImGui::PushItemWidth(80);

        static double min_length{0.001};
        ImGui::InputDouble("Min. Edge Length", &min_length, 0, 0, "%g");

        static double max_length{0.05};
        ImGui::InputDouble("Max. Edge Length", &max_length, 0, 0, "%g");

        static double max_error{0.0005};
        ImGui::InputDouble("Max. Error", &max_error, 0, 0, "%g");

        static int n_iterations{10};
        ImGui::SliderInt("Iterations##adaptive", &n_iterations, 1, 20);

        static bool use_projection{true};
        ImGui::Checkbox("Use Projection##adaptive", &use_projection);

        static bool scale_lengths{true};
        ImGui::Checkbox("Scale Lengths##adaptive", &scale_lengths);

        ImGui::Spacing();

        if (ImGui::Button("Remesh##adaptive"))
        {
            auto scaling = scale_lengths ? bounds(mesh_).size() : 1.0;
            try
            {
                adaptive_remeshing(mesh_, min_length * scaling,
                                   max_length * scaling, max_error * scaling,
                                   n_iterations, use_projection);
            }
            catch (const InvalidInputException& e)
            {
                std::cerr << e.what() << std::endl;
                return;
            }
            update_mesh();
        }

        ImGui::PopItemWidth();
    }
}

int main(int argc, char** argv)
{
#ifndef __EMSCRIPTEN__
    Viewer window("Remeshing", 800, 600);
    if (argc == 2)
        window.load_mesh(argv[1]);
    return window.run();
#else
    Viewer window("Remeshing", 800, 600);
    window.load_mesh(argc == 2 ? argv[1] : "input.off");
    return window.run();
#endif
}
