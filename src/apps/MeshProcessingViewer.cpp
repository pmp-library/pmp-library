//=============================================================================
// Copyright (C) 2011-2018 The pmp-library developers
// All rights reserved.
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

#include <imgui.h>

using namespace pmp;

//=============================================================================

void MeshProcessingViewer::keyboard(int key, int scancode, int action, int mods)
{
    if (action!=GLFW_PRESS && action!=GLFW_REPEAT)
        return;

    switch (key)
    {
        case GLFW_KEY_F:
        {
            SurfaceFeatures sf(m_mesh);
            sf.detectAngle(70);
            break;
        }
        case GLFW_KEY_O: // change face orientation
        {
            SurfaceMeshGL newMesh;
            for (auto v : m_mesh.vertices())
            {
                newMesh.addVertex(m_mesh.position(v));
            }
            for (auto f : m_mesh.faces())
            {
                std::vector<SurfaceMesh::Vertex> vertices;
                for (auto v : m_mesh.vertices(f))
                {
                    vertices.push_back(v);
                }
                std::reverse(vertices.begin(), vertices.end());
                newMesh.addFace(vertices);
            }
            m_mesh = newMesh;
            updateMesh();
            break;
        }
        case GLFW_KEY_W:
        {
            m_mesh.write("output.off");
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

void MeshProcessingViewer::processImGUI()
{
    MeshViewer::processImGUI();

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Curvature"))
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
            SurfaceSmoothing smoother(m_mesh);
            smoother.explicitSmoothing(iterations);
            updateMesh();
        }

        ImGui::Spacing();

        static float timestep = 0.001;
        float        lb       = 0.001;
        float        ub       = 0.1;
        ImGui::PushItemWidth(100);
        ImGui::SliderFloat("TimeStep", &timestep, lb, ub);
        ImGui::PopItemWidth();

        if (ImGui::Button("Implicit Smoothing"))
        {
            Scalar dt = timestep * m_radius * m_radius;
            SurfaceSmoothing smoother(m_mesh);
            smoother.implicitSmoothing(dt);
            updateMesh();
        }
    }


    ImGui::Spacing();
    ImGui::Spacing();


    if (ImGui::CollapsingHeader("Decimation"))
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


    ImGui::Spacing();
    ImGui::Spacing();


    if (ImGui::CollapsingHeader("Subdivision"))
    {
        if (ImGui::Button("Loop Subdivision"))
        {
            SurfaceSubdivision(m_mesh).loop();
            updateMesh();
        }

        if (ImGui::Button("Sqrt(3) Subdivision"))
        {
            SurfaceSubdivision(m_mesh).sqrt3();
            updateMesh();
        }
    }


    ImGui::Spacing();
    ImGui::Spacing();


    if (ImGui::CollapsingHeader("Remeshing"))
    {
        if (ImGui::Button("Adaptive Remeshing"))
        {
            auto bb = m_mesh.bounds().size();
            SurfaceRemeshing(m_mesh).adaptiveRemeshing(
                    0.001 * bb,  // min length
                    1.0 * bb,    // max length
                    0.001 * bb); // approx. error
            updateMesh();
        }

        if (ImGui::Button("Uniform Remeshing"))
        {
            Scalar l(0);
            for (auto eit : m_mesh.edges())
                l += distance(m_mesh.position(m_mesh.vertex(eit, 0)),
                        m_mesh.position(m_mesh.vertex(eit, 1)));
            l /= (Scalar)m_mesh.nEdges();
            SurfaceRemeshing(m_mesh).uniformRemeshing(l);
            updateMesh();
        }
    }
}


//=============================================================================
