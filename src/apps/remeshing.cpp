//=============================================================================

#include <pmp/gl/MeshViewer.h>
#include <pmp/algorithms/SurfaceFeatures.h>
#include <pmp/algorithms/SurfaceRemeshing.h>
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

    if (ImGui::CollapsingHeader("Remeshing", ImGuiTreeNodeFlags_DefaultOpen))
    {
        static int featureAngle = 70;
        ImGui::PushItemWidth(80);
        ImGui::SliderInt("", &featureAngle, 1, 180);
        ImGui::PopItemWidth();
        ImGui::SameLine();
        if (ImGui::Button("Detect Features"))
        {
            SurfaceFeatures sf(m_mesh);
            sf.clear();
            sf.detectAngle(featureAngle);
            updateMesh();
        }

        ImGui::Text("Remeshing:");
        ImGui::SameLine();

        if (ImGui::Button("Uniform"))
        {
            Scalar l(0);
            for (auto eit : m_mesh.edges())
                l += distance(m_mesh.position(m_mesh.vertex(eit, 0)),
                              m_mesh.position(m_mesh.vertex(eit, 1)));
            l /= (Scalar)m_mesh.nEdges();
            SurfaceRemeshing(m_mesh).uniformRemeshing(l);
            updateMesh();
        }

        ImGui::SameLine();

        if (ImGui::Button("Adaptive"))
        {
            auto bb = m_mesh.bounds().size();
            SurfaceRemeshing(m_mesh).adaptiveRemeshing(
                0.001 * bb,  // min length
                0.100 * bb,  // max length
                0.001 * bb); // approx. error
            updateMesh();
        }
    }
}

//=============================================================================

int main(int argc, char** argv)
{
#ifndef __EMSCRIPTEN__
    Viewer window("Remeshing", 800, 600);
    if (argc == 2)
        window.loadMesh(argv[1]);
    return window.run();
#else
    Viewer window("Remeshing", 800, 600);
    window.loadMesh(argc == 2 ? argv[1] : "input.off");
    return window.run();
#endif
}

//=============================================================================
