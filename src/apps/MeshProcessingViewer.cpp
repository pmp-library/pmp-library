//=============================================================================
// Copyright (C) 2011-2019 The pmp-library developers
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//=============================================================================

#include "MeshProcessingViewer.h"

#include <pmp/algorithms/SurfaceSubdivision.h>
#include <pmp/algorithms/SurfaceFeatures.h>
#include <pmp/algorithms/SurfaceSimplification.h>
#include <pmp/algorithms/SurfaceSmoothing.h>
#include <pmp/algorithms/SurfaceFairing.h>
#include <pmp/algorithms/SurfaceRemeshing.h>
#include <pmp/algorithms/SurfaceCurvature.h>
#include <pmp/algorithms/SurfaceGeodesic.h>

#include <imgui.h>

using namespace pmp;

//=============================================================================

void MeshProcessingViewer::keyboard(int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS && action != GLFW_REPEAT)
        return;

    switch (key)
    {
        case GLFW_KEY_F:
        {
            SurfaceFeatures sf(mesh_);
            sf.detect_angle(70);
            break;
        }

        case GLFW_KEY_O: // change face orientation
        {
            SurfaceMeshGL new_mesh;
            for (auto v : mesh_.vertices())
            {
                new_mesh.add_vertex(mesh_.position(v));
            }
            for (auto f : mesh_.faces())
            {
                std::vector<SurfaceMesh::Vertex> vertices;
                for (auto v : mesh_.vertices(f))
                {
                    vertices.push_back(v);
                }
                std::reverse(vertices.begin(), vertices.end());
                new_mesh.add_face(vertices);
            }
            mesh_ = new_mesh;
            update_mesh();
            break;
        }
        case GLFW_KEY_W:
        {
            mesh_.write("output.off");
            break;
        }
        default:
        {
            MeshViewer::keyboard(key, scancode, action, mods);
            break;
        }
    }
}

//----------------------------------------------------------------------------

void MeshProcessingViewer::process_imgui()
{
    MeshViewer::process_imgui();

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Curvature"))
    {
        if (ImGui::Button("Mean Curvature"))
        {
            SurfaceCurvature analyzer(mesh_);
            analyzer.analyze_tensor(1, true);
            analyzer.mean_curvature_to_texture_coordinates();
            mesh_.use_cold_warm_texture();
            update_mesh();
            set_draw_mode("Texture");
        }
        if (ImGui::Button("Gauss Curvature"))
        {
            SurfaceCurvature analyzer(mesh_);
            analyzer.analyze_tensor(1, true);
            analyzer.gauss_curvature_to_texture_coordinates();
            mesh_.use_cold_warm_texture();
            update_mesh();
            set_draw_mode("Texture");
        }
        if (ImGui::Button("Abs. Max. Curvature"))
        {
            SurfaceCurvature analyzer(mesh_);
            analyzer.analyze_tensor(1, true);
            analyzer.max_curvature_to_texture_coordinates();
            mesh_.use_cold_warm_texture();
            update_mesh();
            set_draw_mode("Texture");
        }
    }

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Smoothing"))
    {
        static int iterations = 10;
        ImGui::PushItemWidth(100);
        ImGui::SliderInt("Iterations", &iterations, 1, 100);
        ImGui::PopItemWidth();

        if (ImGui::Button("Explicit Smoothing"))
        {
            SurfaceSmoothing smoother(mesh_);
            smoother.explicit_smoothing(iterations);
            update_mesh();
        }

        ImGui::Spacing();

        static float timestep = 0.001;
        float lb = 0.001;
        float ub = 0.1;
        ImGui::PushItemWidth(100);
        ImGui::SliderFloat("TimeStep", &timestep, lb, ub);
        ImGui::PopItemWidth();

        if (ImGui::Button("Implicit Smoothing"))
        {
            Scalar dt = timestep * radius_ * radius_;
            SurfaceSmoothing smoother(mesh_);
            smoother.implicit_smoothing(dt);
            update_mesh();
        }
    }

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Decimation"))
    {
        static int target_percentage = 10;
        ImGui::PushItemWidth(100);
        ImGui::SliderInt("Percentage", &target_percentage, 1, 99);
        ImGui::PopItemWidth();

        static int normal_deviation = 180;
        ImGui::PushItemWidth(100);
        ImGui::SliderInt("Normal Deviation", &normal_deviation, 1, 180);
        ImGui::PopItemWidth();

        static int aspect_ratio = 10;
        ImGui::PushItemWidth(100);
        ImGui::SliderInt("Aspect Ratio", &aspect_ratio, 1, 10);
        ImGui::PopItemWidth();

        if (ImGui::Button("Decimate it!"))
        {
            SurfaceSimplification ss(mesh_);
            ss.initialize(aspect_ratio, 0.0, 0.0, normal_deviation, 0.0);
            ss.simplify(mesh_.n_vertices() * 0.01 * target_percentage);
            update_mesh();
        }
    }

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Subdivision"))
    {
        if (ImGui::Button("Loop Subdivision"))
        {
            SurfaceSubdivision(mesh_).loop();
            update_mesh();
        }

        if (ImGui::Button("Sqrt(3) Subdivision"))
        {
            SurfaceSubdivision(mesh_).sqrt3();
            update_mesh();
        }
    }

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Remeshing"))
    {
        if (ImGui::Button("Adaptive Remeshing"))
        {
            auto bb = mesh_.bounds().size();
            SurfaceRemeshing(mesh_).adaptive_remeshing(
                0.001 * bb,  // min length
                1.0 * bb,    // max length
                0.001 * bb); // approx. error
            update_mesh();
        }

        if (ImGui::Button("Uniform Remeshing"))
        {
            Scalar l(0);
            for (auto eit : mesh_.edges())
                l += distance(mesh_.position(mesh_.vertex(eit, 0)),
                              mesh_.position(mesh_.vertex(eit, 1)));
            l /= (Scalar)mesh_.n_edges();
            SurfaceRemeshing(mesh_).uniform_remeshing(l);
            update_mesh();
        }
    }
}

//----------------------------------------------------------------------------

void MeshProcessingViewer::mouse(int button, int action, int mods)
{
    if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_MIDDLE)
    {
        double x, y;
        cursor_pos(x, y);
        SurfaceMesh::Vertex v = pick_vertex(x, y);
        if (mesh_.is_valid(v))
        {
            // setup seed
            std::vector<SurfaceMesh::Vertex>  seed;
            seed.push_back(v);

            // compute geodesic distance
            SurfaceGeodesic geodist(mesh_, seed);

            // setup texture coordinates for visualization
            geodist.distance_to_texture_coordinates();
            mesh_.use_checkerboard_texture();
            update_mesh();
            set_draw_mode("Texture");
        }
    }
    else
    {
        MeshViewer::mouse(button, action, mods);
    }
}

//=============================================================================
