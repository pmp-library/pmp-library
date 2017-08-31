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

#include "MeshViewer.h"

#include "cold_warm_texture.h"

#include <cfloat>
#include <iostream>
#include <sstream>

//=============================================================================

namespace surface_mesh {

//=============================================================================

MeshViewer::MeshViewer(const char* title, int width, int height)
    : TrackballViewer(title, width, height)
{
    // setup draw modes
    clearDrawModes();
    addDrawMode("Points");
    addDrawMode("Hidden Line");
    addDrawMode("Smooth Shading");
    setDrawMode("Smooth Shading");
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
        setScene(bb.center(), 0.5*bb.size());

        // compute face & vertex normals, update face indices
        updateMesh();

        std::cout << "Load " << filename << ": "
            << m_mesh.nVertices() << " vertices, "
            << m_mesh.nFaces() << " faces\n";

        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------

void MeshViewer::updateMesh()
{
    // re-compute face and vertex normals
    m_mesh.updateFaceNormals();
    m_mesh.updateVertexNormals();
    m_mesh.updateOpenGLBuffers();
}

//-----------------------------------------------------------------------------

void MeshViewer::draw(const std::string& drawMode)
{
    if (m_mesh.isEmpty())
        return;


    // setup matrices
    mat4 mv_matrix  = m_modelviewMatrix;
    mat4 mvp_matrix = m_projectionMatrix * m_modelviewMatrix;
    mat3 n_matrix   = inverse(transpose(mat3(mv_matrix)));


    // render filled surface triangles with Phong lighting
    m_phongShader.use();
    m_phongShader.set_uniform("modelview_projection_matrix", mvp_matrix);
    m_phongShader.set_uniform("modelview_matrix", mv_matrix);
    m_phongShader.set_uniform("normal_matrix", n_matrix);
    m_phongShader.set_uniform("light1", normalize(vec3( 1.0, 1.0, 1.0)));
    m_phongShader.set_uniform("light2", normalize(vec3(-1.0, 1.0, 1.0)));
    m_phongShader.set_uniform("color", vec3(0.45, 0.5, 0.55));


    if (drawMode == "Points")
    {
        m_mesh.drawPoints();
    }

    else if (drawMode == "Hidden Line")
    {
        // draw faces
        glDepthRange(0.01, 1.0);
        m_mesh.drawFaces();

        // overlay edges
        glDepthRange(0.0, 1.0);
        glDepthFunc(GL_LEQUAL);
        m_phongShader.set_uniform("color", vec3(0.1, 0.1, 0.1));
        m_mesh.drawEdges();
        glDepthFunc(GL_LESS);
    }

    else if (drawMode == "Smooth Shading")
    {
        m_mesh.drawFaces();
    }


    glCheckError();
}


//=============================================================================
} // namespace surface_mesh
//=============================================================================
