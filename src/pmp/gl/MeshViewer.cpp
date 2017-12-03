//=============================================================================
// Copyright (C) 2011-2017 The pmp-library developers
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

#include "MeshViewer.h"

#include <imgui.h>

#include <cfloat>
#include <iostream>
#include <sstream>

//=============================================================================

namespace pmp {

//=============================================================================

MeshViewer::MeshViewer(const char* title, int width, int height, bool showgui)
    : TrackballViewer(title, width, height, showgui)
{
    // setup draw modes
    clearDrawModes();
    addDrawMode("Points");
    addDrawMode("Hidden Line");
    addDrawMode("Smooth Shading");
    addDrawMode("Scalar Field");
    setDrawMode("Smooth Shading");

    m_creaseAngle = 90.0;
}

//-----------------------------------------------------------------------------

MeshViewer::~MeshViewer()
{
}

//-----------------------------------------------------------------------------

bool MeshViewer::loadMesh(const char* filename)
{
    // load mesh
    if (m_mesh.read(filename))
    {
        // update scene center and bounds
        BoundingBox bb = m_mesh.bounds();
        setScene(bb.center(), 0.5 * bb.size());

        // compute face & vertex normals, update face indices
        updateMesh();

        std::cout << "Load " << filename << ": " << m_mesh.nVertices()
                  << " vertices, " << m_mesh.nFaces() << " faces\n";

        m_filename = filename;
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------

void MeshViewer::updateMesh()
{
    // re-compute face and vertex normals
    m_mesh.updateOpenGLBuffers();
}

//-----------------------------------------------------------------------------

void MeshViewer::processImGUI()
{
    if (ImGui::CollapsingHeader("Mesh Info", ImGuiTreeNodeFlags_DefaultOpen))
    {
        // output mesh statistics
        ImGui::BulletText("%d vertices", (int)m_mesh.nVertices());
        ImGui::BulletText("%d edges", (int)m_mesh.nEdges());
        ImGui::BulletText("%d faces", (int)m_mesh.nFaces());

        // control crease angle
        ImGui::PushItemWidth(100);
        ImGui::SliderFloat("Crease Angle", &m_creaseAngle, 0.0f, 180.0f, "%.0f");
        ImGui::PopItemWidth();
        if (m_creaseAngle != m_mesh.creaseAngle())
        {
            m_mesh.setCreaseAngle(m_creaseAngle);
        }
    }
}

//-----------------------------------------------------------------------------

void MeshViewer::draw(const std::string& drawMode)
{
    // draw mesh
    m_mesh.draw(m_projectionMatrix, m_modelviewMatrix, drawMode);
}

//-----------------------------------------------------------------------------
//
void MeshViewer::keyboard(GLFWwindow* window, int key, int scancode, int action,
                          int mods)
{
    if (action != GLFW_PRESS) // only react on key press events
        return;

    switch (key)
    {
        case GLFW_KEY_BACKSPACE: // reload model
        {
            loadMesh(m_filename.c_str());
            break;
        }

        case GLFW_KEY_C: // adjust crease angle
        {
            if (mods & GLFW_MOD_SHIFT)
                m_mesh.setCreaseAngle(m_mesh.creaseAngle() + 10);
            else
                m_mesh.setCreaseAngle(m_mesh.creaseAngle() - 10);
            m_creaseAngle = m_mesh.creaseAngle();

            std::cout << "crease angle: " << m_mesh.creaseAngle() << std::endl;
            break;
        }

        case GLFW_KEY_O: // write mesh
        {
            m_mesh.write("output.off");
            break;
        }

        default:
        {
            TrackballViewer::keyboard(window, key, scancode, action, mods);
            break;
        }
    }
}

//=============================================================================
} // namespace pmp
//=============================================================================
