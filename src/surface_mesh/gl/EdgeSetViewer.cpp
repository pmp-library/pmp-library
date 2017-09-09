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

#include "EdgeSetViewer.h"

#include <cfloat>
#include <iostream>
#include <sstream>

//=============================================================================

namespace surface_mesh {

//=============================================================================

EdgeSetViewer::EdgeSetViewer(const char* title, int width, int height)
    : TrackballViewer(title, width, height)
{
    // setup draw modes
    clearDrawModes();
    addDrawMode("Wireframe");
    setDrawMode("Wireframe");
}

//-----------------------------------------------------------------------------

EdgeSetViewer::~EdgeSetViewer()
{
}

//-----------------------------------------------------------------------------

bool EdgeSetViewer::loadEdgeSet(const char* filename)
{
    // load edge set
    if (m_edgeSet.read(filename))
    {
        // update scene center and bounds
        BoundingBox bb = m_edgeSet.bounds();
        setScene(bb.center(), 0.5*bb.size());

        updateEdgeSet();

        std::cout << "Load " << filename << ": "
            << m_edgeSet.nVertices() << " vertices, "
            << m_edgeSet.nEdges() << " edges\n";

        m_filename = filename;
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------

void EdgeSetViewer::updateEdgeSet()
{
    // update OpenGL buffers
    m_edgeSet.updateOpenGLBuffers();
}

//-----------------------------------------------------------------------------

void EdgeSetViewer::draw(const std::string& drawMode)
{
    m_edgeSet.draw(m_projectionMatrix, m_modelviewMatrix, drawMode);
}

//-----------------------------------------------------------------------------
//
void EdgeSetViewer::keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS) // only react on key press events
        return;

    switch (key)
    {
        case GLFW_KEY_BACKSPACE: // reload model
        {
            loadEdgeSet(m_filename.c_str());
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
} // namespace surface_mesh
//=============================================================================
