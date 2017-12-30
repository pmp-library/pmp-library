//=============================================================================

#include <pmp/gl/MeshViewer.h>
#include <pmp/algorithms/SurfaceCurvature.h>
#include <pmp/algorithms/SurfaceSmoothing.h>
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

    if (ImGui::CollapsingHeader("Smoothing", ImGuiTreeNodeFlags_DefaultOpen))
    {
        static int weight = 0;
        ImGui::RadioButton("Cotan Laplace",  &weight, 0);
        ImGui::RadioButton("Uniform Laplace", &weight, 1);
        bool uniformLaplace = (weight == 1);

        static int iterations=10;
        ImGui::PushItemWidth(100);
        ImGui::SliderInt("Iterations", &iterations, 1, 100);
        ImGui::PopItemWidth();

        if (ImGui::Button("Explicit Smoothing"))
        {
            SurfaceSmoothing smoother(m_mesh);
            smoother.explicitSmoothing(iterations, uniformLaplace);
            updateMesh();
        }

        ImGui::Spacing();
        ImGui::Spacing();

        static float timestep = 0.001;
        float lb = uniformLaplace ? 1.0 : 0.001;
        float ub = uniformLaplace ? 100.0 : 0.1;
        ImGui::PushItemWidth(100);
        ImGui::SliderFloat("TimeStep", &timestep, lb, ub);
        ImGui::PopItemWidth();

        if (ImGui::Button("Implicit Smoothing"))
        {
            Scalar dt = uniformLaplace ? timestep : timestep*m_radius*m_radius;
            SurfaceSmoothing smoother(m_mesh);
            smoother.implicitSmoothing(dt, uniformLaplace);
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
