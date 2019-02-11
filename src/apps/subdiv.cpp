//=============================================================================

#include <pmp/gl/MeshViewer.h>
#include <pmp/algorithms/SurfaceSubdivision.h>
#include <pmp/algorithms/SurfaceFeatures.h>
#include <imgui.h>

using namespace pmp;

//=============================================================================

class Viewer : public MeshViewer
{
public:
    Viewer(const char* title, int width, int height, bool showgui = true);

protected:
    virtual void process_imgui();
};

//=============================================================================

Viewer::Viewer(const char* title, int width, int height, bool showgui)
    : MeshViewer(title, width, height, showgui)
{
    set_draw_mode("Hidden Line");
}

//=============================================================================

void Viewer::process_imgui()
{
    MeshViewer::process_imgui();

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Subdivision", ImGuiTreeNodeFlags_DefaultOpen))
    {
        //static int featureAngle = 70;
        //ImGui::PushItemWidth(80);
        //ImGui::SliderInt("", &featureAngle, 1, 180);
        //ImGui::PopItemWidth();
        //ImGui::SameLine();
        //if (ImGui::Button("Detect Features"))
        //{
        //SurfaceFeatures sf(mesh_);
        //sf.clear();
        //sf.detect_angle(featureAngle);
        //update_mesh();
        //}

        if (ImGui::Button("Triangulate Mesh"))
        {
            mesh_.triangulate();
            update_mesh();
        }

        if (ImGui::Button("Loop Subdivision"))
        {
            SurfaceSubdivision(mesh_).loop();
            update_mesh();
        }

        //if (ImGui::Button("Sqrt(3) Subdivision"))
        //{
        //SurfaceSubdivision(mesh_).sqrt3();
        //update_mesh();
        //}

        if (ImGui::Button("Catmull-Clark Subdivision"))
        {
            SurfaceSubdivision(mesh_).catmull_clark();
            update_mesh();
        }
    }
}

//=============================================================================

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

//=============================================================================
