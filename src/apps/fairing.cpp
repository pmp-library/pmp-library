//=============================================================================

#include <pmp/gl/MeshViewer.h>
#include <pmp/algorithms/SurfaceCurvature.h>
#include <pmp/algorithms/SurfaceFairing.h>
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
    : MeshViewer(title,width, height)
{
}

//=============================================================================

void Viewer::processImGUI()
{
    MeshViewer::processImGUI();

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Curvature", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::Button("Mean Curvature"))
        {
            SurfaceCurvature analyzer(m_mesh);
            analyzer.analyzeTensor(1, true);
            analyzer.meanCurvatureToTextureCoordinates();
            updateMesh();
            m_mesh.useColdWarmTexture();
            setDrawMode("Texture");
        }
    }

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Fairing", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::Button("Minimize Area"))
        {
            SurfaceFairing fair(m_mesh);
            fair.minimizeArea();
            updateMesh();
        }
        if (ImGui::Button("Minimize Curvature"))
        {
            SurfaceFairing fair(m_mesh);
            fair.minimizeCurvature();
            updateMesh();
        }
        if (ImGui::Button("Minimize Curvature Variation"))
        {
            SurfaceFairing fair(m_mesh);
            fair.fair(3);
            updateMesh();
        }
    }
}

//=============================================================================

int main(int argc, char **argv)
{
#ifndef __EMSCRIPTEN__
    Viewer window("Decimation", 800, 600);
    if (argc == 2) window.loadMesh(argv[1]);
    return window.run();
#else
    Viewer window("Decimation", 800, 600);
    window.loadMesh(argc==2 ? argv[1] : "input.off");
    return window.run();
#endif
}

//=============================================================================
