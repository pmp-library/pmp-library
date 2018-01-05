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
    virtual void processImGUI();
};

//=============================================================================

Viewer::Viewer(const char* title, int width, int height, bool showgui)
    : MeshViewer(title, width, height, showgui)
{
    setDrawMode("Hidden Line");
}

//=============================================================================

void Viewer::processImGUI()
{
    MeshViewer::processImGUI();

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
        //SurfaceFeatures sf(m_mesh);
        //sf.clear();
        //sf.detectAngle(featureAngle);
        //updateMesh();
        //}

        if (ImGui::Button("Triangulate Mesh"))
        {
            m_mesh.triangulate();
            updateMesh();
        }

        if (ImGui::Button("Loop Subdivision"))
        {
            SurfaceSubdivision(m_mesh).loop();
            updateMesh();
        }

        //if (ImGui::Button("Sqrt(3) Subdivision"))
        //{
        //SurfaceSubdivision(m_mesh).sqrt3();
        //updateMesh();
        //}

        if (ImGui::Button("Catmull-Clark Subdivision"))
        {
            SurfaceSubdivision(m_mesh).catmullClark();
            updateMesh();
        }
    }
}

//=============================================================================

int main(int argc, char** argv)
{
#ifndef __EMSCRIPTEN__
    Viewer window("Subdivision", 800, 600);
    if (argc == 2)
        window.loadMesh(argv[1]);
    return window.run();
#else
    Viewer window("Subdivision", 800, 600);
    window.loadMesh(argc == 2 ? argv[1] : "input.obj");
    return window.run();
#endif
}

//=============================================================================
