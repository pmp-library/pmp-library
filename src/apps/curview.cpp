//=============================================================================

#include <pmp/gl/MeshViewer.h>
#include <pmp/algorithms/SurfaceCurvature.h>
#include <imgui.h>

using namespace pmp;

//=============================================================================

class Viewer : public MeshViewer
{
public:
    Viewer(const char* title, int width, int height, bool showgui)
        : MeshViewer(title, width, height, showgui)
    {
        setDrawMode("Solid Smooth");
    }

protected:

    void processImGUI()
    {
        //MeshViewer::processImGUI();

        ImGui::SetNextWindowPos(ImVec2(10,10), ImGuiCond_Once);
        ImGui::Begin("Curvature", NULL, ImGuiWindowFlags_AlwaysAutoResize);

        if (ImGui::Button("Mean Curvature"))
        {
            SurfaceCurvature analyzer(m_mesh);
            analyzer.analyzeTensor(1, true);
            analyzer.meanCurvatureToTextureCoordinates();
            updateMesh();
            setDrawMode("Scalar Field");
        }
        if (ImGui::Button("Gauss Curvature"))
        {
            SurfaceCurvature analyzer(m_mesh);
            analyzer.analyzeTensor(1, true);
            analyzer.gaussCurvatureToTextureCoordinates();
            updateMesh();
            setDrawMode("Scalar Field");
        }
        if (ImGui::Button("Abs. Max. Curvature"))
        {
            SurfaceCurvature analyzer(m_mesh);
            analyzer.analyzeTensor(1, true);
            analyzer.maxCurvatureToTextureCoordinates();
            updateMesh();
            setDrawMode("Scalar Field");
        }

        ImGui::End();
    }
};

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
