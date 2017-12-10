//=============================================================================

#include <pmp/gl/MeshViewer.h>
#include <pmp/algorithms/SurfaceCurvature.h>
#include <imgui.h>

using namespace pmp;

//=============================================================================

class Viewer : public MeshViewer
{
public:
    Viewer(const char* title, int width, int height, bool showgui);
protected:
    virtual void processImGUI();
};

//=============================================================================

Viewer::Viewer(const char* title, int width, int height, bool showgui)
    : MeshViewer(title, width, height, showgui)
{
    setDrawMode("Solid Smooth");
}

//=============================================================================

void Viewer::processImGUI()
{
    MeshViewer::processImGUI();

    if (ImGui::CollapsingHeader("Curvature", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::Button("Mean Curvature"))
        {
            SurfaceCurvature analyzer(m_mesh);
            analyzer.analyzeTensor(1, true);
            analyzer.meanCurvatureToTextureCoordinates();
            m_mesh.useColdWarmTexture();
            updateMesh();
            setDrawMode("Texture");
        }
        if (ImGui::Button("Gauss Curvature"))
        {
            SurfaceCurvature analyzer(m_mesh);
            analyzer.analyzeTensor(1, true);
            analyzer.gaussCurvatureToTextureCoordinates();
            m_mesh.useColdWarmTexture();
            updateMesh();
            setDrawMode("Texture");
        }
        if (ImGui::Button("Abs. Max. Curvature"))
        {
            SurfaceCurvature analyzer(m_mesh);
            analyzer.analyzeTensor(1, true);
            analyzer.maxCurvatureToTextureCoordinates();
            m_mesh.useColdWarmTexture();
            updateMesh();
            setDrawMode("Texture");
        }
    }
}

//=============================================================================

int main(int argc, char **argv)
{
#ifndef __EMSCRIPTEN__
    Viewer window("Curvature", 800, 600, true);
    if (argc == 2) window.loadMesh(argv[1]);
    return window.run();
#else
    Viewer window("Curvature", 800, 600, true);
    window.loadMesh(argc==2 ? argv[1] : "input.off");
    return window.run();
#endif
}

//=============================================================================
