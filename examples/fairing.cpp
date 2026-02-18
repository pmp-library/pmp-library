// Copyright 2011-2019 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#include <pmp/viewers/mesh_viewer.h>
#include <pmp/algorithms/curvature.h>
#include <pmp/algorithms/fairing.h>
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
            curvature(mesh_, Curvature::Mean, 1, true, true);
            curvature_to_texture_coordinates(mesh_);
            update_mesh();
            renderer_.use_cold_warm_texture();
            set_draw_mode("Texture");
        }
    }

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Fairing", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::Button("Minimize Area"))
        {
            try
            {
                minimize_area(mesh_);
            }
            catch (const std::exception& e)
            {
                std::cerr << e.what() << std::endl;
                return;
            }
            update_mesh();
        }
        if (ImGui::Button("Minimize Curvature"))
        {
            try
            {
                minimize_curvature(mesh_);
            }
            catch (const std::exception& e)
            {
                std::cerr << e.what() << std::endl;
                return;
            }
            update_mesh();
        }
        if (ImGui::Button("Minimize Curvature Variation"))
        {
            try
            {
                fair(mesh_, 3);
            }
            catch (const std::exception& e)
            {
                std::cerr << e.what() << std::endl;
                return;
            }
            update_mesh();
        }
    }
}

int main(int argc, char** argv)
{
#ifndef __EMSCRIPTEN__
    Viewer window("Fairing", 800, 600);
    if (argc == 2)
        window.load_mesh(argv[1]);
    return window.run();
#else
    Viewer window("Fairing", 800, 600);
    window.load_mesh(argc == 2 ? argv[1] : "input.off");
    return window.run();
#endif
}
