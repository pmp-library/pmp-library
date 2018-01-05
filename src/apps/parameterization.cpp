//=============================================================================

#include <pmp/gl/MeshViewer.h>
#include <pmp/algorithms/SurfaceParameterization.h>
#include <imgui.h>

using namespace pmp;

//=============================================================================

class Viewer : public MeshViewer
{
public:
    Viewer(const char* title, int width, int height);

protected:
    virtual void processImGUI() override;
    virtual void draw(const std::string& _draw_mode) override;

private:
    SurfaceParameterization m_param;
};

//=============================================================================

Viewer::Viewer(const char* title, int width, int height)
    : MeshViewer(title, width, height), m_param(m_mesh)
{
}

//=============================================================================

void Viewer::processImGUI()
{
    MeshViewer::processImGUI();

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Parametrization",
                                ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Spacing();
        if (ImGui::Button("Discrete Harmonic Param"))
        {
            m_param.harmonic();
            m_mesh.useCheckerboardTexture();
            setDrawMode("Texture");
            updateMesh();
        }

        ImGui::Spacing();
        if (ImGui::Button("Least Squares Conformal Map"))
        {
            m_param.lscm();
            m_mesh.useCheckerboardTexture();
            setDrawMode("Texture");
            updateMesh();
        }
    }
}

//=============================================================================

void Viewer::draw(const std::string& drawMode)
{
    // normal mesh draw
    glViewport(0, 0, m_width, m_height);
    m_mesh.draw(m_projectionMatrix, m_modelviewMatrix, drawMode);

    // draw uv layout
    {
        // clear depth buffer
        glClear(GL_DEPTH_BUFFER_BIT);

        // setup viewport
        GLint size = std::min(m_width, m_height) / 4;
        glViewport(m_width - size - 1, m_height - size - 1, size, size);

        // setup matrices
        mat4 P = orthoMatrix(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f);
        mat4 M = mat4::identity();

        // draw mesh once more
        m_mesh.draw(P, M, "Texture Layout");
    }
}

//=============================================================================

int main(int argc, char** argv)
{
#ifndef __EMSCRIPTEN__
    Viewer window("Parametrization", 800, 600);
    if (argc == 2)
        window.loadMesh(argv[1]);
    return window.run();
#else
    Viewer window("Parametrization", 800, 600);
    window.loadMesh(argc == 2 ? argv[1] : "input.off");
    return window.run();
#endif
}

//=============================================================================
