// Copyright 2011-2022 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#include <pmp/visualization/mesh_viewer.h>
#include <pmp/algorithms/subdivision.h>
#include <pmp/algorithms/triangulation.h>
#include <imgui.h>

using namespace pmp;

class Viewer : public MeshViewer
{
public:
    Viewer(const char* title, int width, int height, bool showgui = true);

protected:
    void process_imgui() override;

private:
    bool show_error_{false};
    std::string error_message_{};
    void show_error();
};

Viewer::Viewer(const char* title, int width, int height, bool showgui)
    : MeshViewer(title, width, height, showgui)
{
    set_draw_mode("Hidden Line");
    crease_angle_ = 0.0;
}

void Viewer::process_imgui()
{
    MeshViewer::process_imgui();

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Subdivision", ImGuiTreeNodeFlags_DefaultOpen))
    {
        static bool preserve_boundary = false;
        ImGui::Checkbox("Preserve Boundary", &preserve_boundary);

        const auto boundary_handling = preserve_boundary
                                           ? BoundaryHandling::Preserve
                                           : BoundaryHandling::Interpolate;

        if (ImGui::Button("Triangulate Mesh"))
        {
            triangulate(mesh_);
            update_mesh();
        }

        if (ImGui::Button("Loop Subdivision"))
        {
            // avoid throwing an exception, directly show error message
            if (!mesh_.is_triangle_mesh())
            {
                error_message_ = "Loop subdivision requires a triangle mesh.\n";
                error_message_ += "Triangulate the mesh first.\n";
                show_error_ = true;
            }
            else
            {
                loop_subdivision(mesh_, boundary_handling);
                update_mesh();
            }
        }

        if (ImGui::Button("Catmull-Clark Subdivision"))
        {
            catmull_clark_subdivision(mesh_, boundary_handling);
            update_mesh();
        }

        if (ImGui::Button("Quad/Tri Subdivision"))
        {
            quad_tri_subdivision(mesh_, boundary_handling);
            update_mesh();
        }

        if (ImGui::Button("Linear subdivision"))
        {
            linear_subdivision(mesh_);
            update_mesh();
        }
    }

    if (show_error_)
        show_error();
}

void Viewer::show_error()
{
    // no transparency
    auto window_color = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
    auto title_color = ImGui::GetStyle().Colors[ImGuiCol_TitleBgActive];
    window_color.w = 1.0;
    title_color.w = 1.0;
    ImGui::PushStyleColor(ImGuiCol_WindowBg, window_color);
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, title_color);

    // centered message box
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(),
                            ImGuiCond_Always, ImVec2(0.5f, 1.0f));
    if (!ImGui::Begin("Error", &show_error_,
                      ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse))
    {
        ImGui::PopStyleColor();
        ImGui::End();
        return;
    }
    ImGui::Text("%s", error_message_.c_str());
    ImGui::Spacing();
    if (ImGui::Button("Ok"))
    {
        show_error_ = false;
    }

    ImGui::PopStyleColor(2);
    ImGui::End();
}

int main(int argc, char** argv)
{
#ifndef __EMSCRIPTEN__
    Viewer window("Subdivision", 800, 600);
    if (argc == 2)
        window.load_mesh(argv[1]);
    return window.run();
#else
    Viewer window("Subdivision", 800, 600);
    window.load_mesh(argc == 2 ? argv[1] : "input.obj");
    return window.run();
#endif
}
