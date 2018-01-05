//=============================================================================

#include <pmp/gl/MeshViewer.h>
#include <pmp/algorithms/SurfaceSimplification.h>
#include <imgui.h>

using namespace pmp;

//=============================================================================

class Viewer : public MeshViewer
{
public:
    Viewer(const char* title, int width, int height);

protected:
    virtual void processImGUI();
};

//=============================================================================

Viewer::Viewer(const char* title, int width, int height)
    : MeshViewer(title, width, height)
{
    setDrawMode("Hidden Line");
    m_creaseAngle = 0.0;
}

//=============================================================================

void Viewer::processImGUI()
{
    MeshViewer::processImGUI();

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Decimation", ImGuiTreeNodeFlags_DefaultOpen))
    {
        static int targetPercentage = 10;
        ImGui::PushItemWidth(100);
        ImGui::SliderInt("Percentage", &targetPercentage, 1, 99);
        ImGui::PopItemWidth();

        static int normalDeviation = 180;
        ImGui::PushItemWidth(100);
        ImGui::SliderInt("Normal Deviation", &normalDeviation, 1, 180);
        ImGui::PopItemWidth();

        static int aspectRatio = 10;
        ImGui::PushItemWidth(100);
        ImGui::SliderInt("Aspect Ratio", &aspectRatio, 1, 10);
        ImGui::PopItemWidth();

        if (ImGui::Button("Decimate it!"))
        {
            SurfaceSimplification ss(m_mesh);
            ss.initialize(aspectRatio, 0.0, 0.0, normalDeviation, 0.0);
            ss.simplify(m_mesh.nVertices() * 0.01 * targetPercentage);
            updateMesh();
        }
    }
}

//=============================================================================

int main(int argc, char** argv)
{
#ifndef __EMSCRIPTEN__
    Viewer window("Decimation", 800, 600);
    if (argc == 2)
        window.loadMesh(argv[1]);
    return window.run();
#else
    Viewer window("Decimation", 800, 600);
    window.loadMesh(argc == 2 ? argv[1] : "input.off");
    return window.run();
#endif
}

//=============================================================================
