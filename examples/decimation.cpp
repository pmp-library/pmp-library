// Copyright 2011-2019 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#include <pmp/visualization/mesh_viewer.h>
#include <pmp/algorithms/decimation.h>
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

    if (ImGui::CollapsingHeader("Decimation", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Spacing();
        ImGui::PushItemWidth(80);

        static int target_percentage = 10;
        ImGui::SliderInt("Number of Vertices (%)", &target_percentage, 1, 99);

        static int normal_deviation = 180;
        ImGui::SliderInt("Normal Deviation", &normal_deviation, 1, 180);

        static int aspect_ratio = 10;
        ImGui::SliderInt("Aspect Ratio", &aspect_ratio, 1, 10);

        static double edge_length = 0.0;
        static int max_valence = 0;
        static double hausdorff_error = 0.0;
        static double seam_threshold = 0.0;
        static double seam_angle_deviation = 0.0;

        ImGui::Spacing();
        ImGui::Indent(3);
        if (ImGui::CollapsingHeader("Advanced Options"))
        {
            ImGui::Spacing();
            ImGui::Indent(10);

            ImGui::InputDouble("Min. Edge Length", &edge_length, 0.0, 0.0,
                               "%g");
            ImGui::SliderInt("Max. Valence", &max_valence, 0, 15);
            ImGui::InputDouble("Max. Hausdorff Error", &hausdorff_error, 0.0,
                               0.0, "%g");
            ImGui::InputDouble("Texture Seam Threshold", &seam_threshold, 0.0,
                               0.0, "%g");
            ImGui::InputDouble("Texture Seam Deviation", &seam_angle_deviation,
                               0.0, 0.0, "%g");
            ImGui::Unindent(10);
        }
        ImGui::Unindent(3);

        ImGui::Spacing();

        if (ImGui::Button("Decimate"))
        {
            try
            {
                auto nv = mesh_.n_vertices() * 0.01 * target_percentage;
                decimate(mesh_, nv, aspect_ratio, edge_length, max_valence,
                         normal_deviation, hausdorff_error, seam_threshold,
                         seam_angle_deviation);
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
    Viewer window("Decimation", 800, 600);
    if (argc == 2)
        window.load_mesh(argv[1]);
    return window.run();
#else
    Viewer window("Decimation", 800, 600);
    window.load_mesh(argc == 2 ? argv[1] : "input.off");
    return window.run();
#endif
}
