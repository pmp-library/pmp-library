// Copyright 2011-2019 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include <pmp/visualization/mesh_viewer.h>
#include <pmp/algorithms/curvature.h>
#include <imgui.h>

using namespace pmp;

class Viewer : public MeshViewer
{
public:
    Viewer(const char* title, int width, int height, bool showgui);

protected:
    void process_imgui() override;
};

Viewer::Viewer(const char* title, int width, int height, bool showgui)
    : MeshViewer(title, width, height, showgui)
{
    set_draw_mode("Solid Smooth");
}

void Viewer::process_imgui()
{
    MeshViewer::process_imgui();

    if (ImGui::CollapsingHeader("Curvature", ImGuiTreeNodeFlags_DefaultOpen))
    {
        static int iters = 1;
        ImGui::SliderInt("Smoothing Iterations", &iters, 0, 10);
        static bool tensor = true;
        ImGui::Checkbox("Analyze Tensor", &tensor);
        static bool two_ring = true;
        if (tensor)
            ImGui::Checkbox("Two-Ring Neighborhood", &two_ring);

        if (ImGui::Button("Mean Curvature"))
        {
            curvature(mesh_, Curvature::mean, iters, tensor, two_ring);
            curvature_to_texture_coordinates(mesh_);
            renderer_.use_cold_warm_texture();
            update_mesh();
            set_draw_mode("Texture");
        }
        if (ImGui::Button("Gauss Curvature"))
        {
            curvature(mesh_, Curvature::gauss, iters, tensor, two_ring);
            curvature_to_texture_coordinates(mesh_);
            renderer_.use_cold_warm_texture();
            update_mesh();
            set_draw_mode("Texture");
        }
        if (ImGui::Button("Abs. Max. Curvature"))
        {
            curvature(mesh_, Curvature::max_abs, iters, tensor, two_ring);
            curvature_to_texture_coordinates(mesh_);
            renderer_.use_cold_warm_texture();
            update_mesh();
            set_draw_mode("Texture");
        }
    }
}

int main(int argc, char** argv)
{
#ifndef __EMSCRIPTEN__
    Viewer window("Curvature", 800, 600, true);
    if (argc == 2)
        window.load_mesh(argv[1]);
    return window.run();
#else
    Viewer window("Curvature", 800, 600, true);
    window.load_mesh(argc == 2 ? argv[1] : "input.off");
    return window.run();
#endif
}
