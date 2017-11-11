//=============================================================================
// Copyright (C) 2011-2016 by Graphics & Geometry Group, Bielefeld University
// Copyright (C) 2017 Daniel Sieger
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
#ifdef PMP_HAS_EIGEN
#include <pmp/algorithms/SurfaceSmoothing.h>
#endif
#include <pmp/algorithms/SurfaceRemeshing.h>
#include <pmp/algorithms/SurfaceCurvature.h>

#include <imgui.h>

using namespace pmp;

//=============================================================================

void MeshProcessingViewer::keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS) // only react on key press events
        return;

    switch (key)
    {
        case GLFW_KEY_G: // Gauss curvature
        {
            SurfaceCurvature analyzer(m_mesh);
            analyzer.analyzeTensor(1, true);
            analyzer.gaussCurvatureToTextureCoordinates();
            updateMesh();
            setDrawMode("Scalar Field");
            break;
        }
        case GLFW_KEY_M: // mean curvature
        {
            SurfaceCurvature analyzer(m_mesh);
            analyzer.analyzeTensor(1, true);
            analyzer.meanCurvatureToTextureCoordinates();
            updateMesh();
            setDrawMode("Scalar Field");
            break;
        }
        case GLFW_KEY_X: // mean curvature
        {
            SurfaceCurvature analyzer(m_mesh);
            analyzer.analyzeTensor(1, true);
            analyzer.maxCurvatureToTextureCoordinates();
            updateMesh();
            setDrawMode("Scalar Field");
            break;
        }
        case GLFW_KEY_F:
        {
            SurfaceFeatures sf(m_mesh);
            sf.detectAngle(70);
            break;
        }
#ifdef PMP_HAS_EIGEN
        case GLFW_KEY_I:
        {
            // auto bb = m_mesh.bounds();
            // Scalar yrange = bb.max()[1] - bb.min()[1];
            // auto vselected = m_mesh.vertexProperty<bool>("v:selected",false);
            // for (auto v : m_mesh.vertices())
            // {
            //     auto p = m_mesh.position(v);
            //     if (p[1] >= (bb.max()[1] - 0.2*yrange))
            //     {
            //         vselected[v] = false;
            //     }
            //     else if (p[1] < (bb.max()[1] - 0.2*yrange) &&
            //              p[1] > (bb.max()[1] - 0.8*yrange))
            //     {
            //         vselected[v] = true;
            //     }
            //     else
            //     {
            //         vselected[v] = false;
            //     }
            // }

            SurfaceSmoothing ss(m_mesh);
            ss.implicitSmooth(1,0.000001);
            //ss.fair();
            updateMesh();
            break;
        }
#endif
        case GLFW_KEY_L:
        {
            if (m_mesh.isTriangleMesh())
                SurfaceSubdivision(m_mesh).loop();
            else
                SurfaceSubdivision(m_mesh).catmullClark();
            updateMesh();
            break;
        }
        case GLFW_KEY_O:
        {
            m_mesh.write("output.off");
            break;
        }
        case GLFW_KEY_R:
        {
            // adaptive remeshing
            auto bb = m_mesh.bounds().size();
            SurfaceRemeshing(m_mesh).adaptiveRemeshing(
                0.001 * bb,  // min length
                1.0 * bb,    // max length
                0.001 * bb); // approx. error
            updateMesh();
            break;
        }
        case GLFW_KEY_S:
        {
            SurfaceSimplification ss(m_mesh);
            ss.initialize(5); // aspect ratio
            ss.simplify(m_mesh.nVertices() * 0.1);
            updateMesh();
            break;
        }
        case GLFW_KEY_T:
        {
            if (!m_mesh.isTriangleMesh())
                m_mesh.triangulate();
            updateMesh();
            break;
        }
        case GLFW_KEY_U:
        {
            Scalar l(0);
            for (auto eit : m_mesh.edges())
                l += distance(m_mesh.position(m_mesh.vertex(eit, 0)),
                              m_mesh.position(m_mesh.vertex(eit, 1)));
            l /= (Scalar)m_mesh.nEdges();
            SurfaceRemeshing(m_mesh).uniformRemeshing(l);
            updateMesh();
            break;
        }
        default:
        {
            MeshViewer::keyboard(window, key, scancode, action, mods);
            break;
        }
    }
}

//----------------------------------------------------------------------------

void MeshProcessingViewer::processImGUI()
{
    MeshViewer::processImGUI();

#ifndef __EMSCRIPTEN__
    ImGui::SetNextWindowSize(ImVec2(0,0));
#endif
    ImGui::Begin("Mesh Processing");

    if (ImGui::Button("Decimate"))
    {
        SurfaceSimplification ss(m_mesh);
        ss.initialize(5); // aspect ratio
        ss.simplify(m_mesh.nVertices() * 0.1);
        updateMesh();
    }

    if (ImGui::Button("Subdivision"))
    {
        if (m_mesh.isTriangleMesh())
            SurfaceSubdivision(m_mesh).loop();
        else
            SurfaceSubdivision(m_mesh).catmullClark();
        updateMesh();
    }

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

    ImGui::End();
}
