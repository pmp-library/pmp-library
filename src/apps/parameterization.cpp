//=============================================================================
// Copyright (C) 2011-2019 The pmp-library developers
//
// This file is part of the Polygon Mesh Processing Library.
// Distributed under the terms of the MIT license, see LICENSE.txt for details.
//
// SPDX-License-Identifier: MIT
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
    virtual void process_imgui() override;
    virtual void draw(const std::string& _draw_mode) override;

private:
    SurfaceParameterization param_;
};

//=============================================================================

Viewer::Viewer(const char* title, int width, int height)
    : MeshViewer(title, width, height), param_(mesh_)
{
}

//=============================================================================

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
            param_.harmonic();
            mesh_.use_checkerboard_texture();
            set_draw_mode("Texture");
            update_mesh();
        }

        ImGui::Spacing();
        if (ImGui::Button("Least Squares Conformal Map"))
        {
            param_.lscm();
            mesh_.use_checkerboard_texture();
            set_draw_mode("Texture");
            update_mesh();
        }
    }
}

//=============================================================================

void Viewer::draw(const std::string& draw_mode)
{
    // normal mesh draw
    glViewport(0, 0, width_, height_);
    mesh_.draw(projection_matrix_, modelview_matrix_, draw_mode);

    // draw uv layout
    {
        // clear depth buffer
        glClear(GL_DEPTH_BUFFER_BIT);

        // setup viewport
        GLint size = std::min(width_, height_) / 4;
        glViewport(width_ - size - 1, height_ - size - 1, size, size);

        // setup matrices
        mat4 P = ortho_matrix(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f);
        mat4 M = mat4::identity();

        // draw mesh once more
        mesh_.draw(P, M, "Texture Layout");
    }

    // reset viewport
    glViewport(0, 0, width_, height_);
}

//=============================================================================

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

//=============================================================================
