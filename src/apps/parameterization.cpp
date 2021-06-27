// Copyright 2011-2021 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include <pmp/visualization/MeshViewer.h>
#include <pmp/algorithms/SurfaceParameterization.h>
#include <imgui.h>

using namespace pmp;

class Viewer : public MeshViewer
{
public:
    Viewer(const char* title, int width, int height);
    virtual void load_mesh(const char* filename) override;

protected:
    virtual void process_imgui() override;
    virtual void draw(const std::string& _draw_mode) override;
};

Viewer::Viewer(const char* title, int width, int height)
    : MeshViewer(title, width, height)
{
}

void Viewer::load_mesh(const char* filename)
{
    try
    {
        MeshViewer::load_mesh(filename);
    }
    catch (const IOException& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return;
    }

    // alloc tex coordinates
    mesh_.vertex_property<TexCoord>("v:tex", TexCoord(0, 0));
    update_mesh();
}

void Viewer::process_imgui()
{
    MeshViewer::process_imgui();

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Parametrization",
                                ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Spacing();
        if (ImGui::Button("Discrete Harmonic Param"))
        {
            try
            {
                SurfaceParameterization param(mesh_);
                param.harmonic();
            }
            catch (const std::exception& e)
            {
                std::cerr << e.what() << std::endl;
                return;
            }
            mesh_.use_checkerboard_texture();
            set_draw_mode("Texture");
            update_mesh();
        }

        ImGui::Spacing();
        if (ImGui::Button("Least Squares Conformal Map"))
        {
            try
            {
                SurfaceParameterization param(mesh_);
                param.lscm();
            }
            catch (const std::exception& e)
            {
                std::cerr << e.what() << std::endl;
                return;
            }
            mesh_.use_checkerboard_texture();
            set_draw_mode("Texture");
            update_mesh();
        }
    }
}

void Viewer::draw(const std::string& draw_mode)
{
    // normal mesh draw
    glViewport(0, 0, width(), height());
    mesh_.draw(projection_matrix_, modelview_matrix_, draw_mode);

    // draw uv layout
    {
        // clear depth buffer
        glClear(GL_DEPTH_BUFFER_BIT);

        // setup viewport
        GLint size = std::min(width(), height()) / 4;
        glViewport(width() - size - 1, height() - size - 1, size, size);

        // setup matrices
        mat4 P = ortho_matrix(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f);
        mat4 M = mat4::identity();

        // draw mesh once more
        mesh_.draw(P, M, "Texture Layout");
    }

    // reset viewport
    glViewport(0, 0, width(), height());
}

int main(int argc, char** argv)
{
#ifndef __EMSCRIPTEN__
    Viewer window("Parametrization", 800, 600);
    if (argc == 2)
        window.load_mesh(argv[1]);
    return window.run();
#else
    Viewer window("Parametrization", 800, 600);
    window.load_mesh(argc == 2 ? argv[1] : "input.off");
    return window.run();
#endif
}
