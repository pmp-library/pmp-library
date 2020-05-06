// Copyright 2011-2019 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include <pmp/visualization/MeshViewer.h>
#include <pmp/algorithms/SurfaceCurvature.h>
#include <pmp/algorithms/SurfaceFairing.h>
#include <imgui.h>

using namespace pmp;

class Viewer : public MeshViewer
{
public:
    Viewer(const char* title, int width, int height);

protected:
    virtual void process_imgui();
};

Viewer::Viewer(const char* title, int width, int height)
    : MeshViewer(title, width, height)
{
}

void Viewer::process_imgui()
{
    MeshViewer::process_imgui();

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Curvature", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::Button("Mean Curvature"))
        {
            SurfaceCurvature analyzer(mesh_);
            analyzer.analyze_tensor(1, true);
            analyzer.mean_curvature_to_texture_coordinates();
            update_mesh();
            mesh_.use_cold_warm_texture();
            set_draw_mode("Texture");
        }
    }

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Fairing", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::Button("Minimize Area"))
        {
            SurfaceFairing fair(mesh_);
            fair.minimize_area();
            update_mesh();
        }
        if (ImGui::Button("Minimize Curvature"))
        {
            SurfaceFairing fair(mesh_);
            fair.minimize_curvature();
            update_mesh();
        }
        if (ImGui::Button("Minimize Curvature Variation"))
        {
            SurfaceFairing fair(mesh_);
            fair.fair(3);
            update_mesh();
        }
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
