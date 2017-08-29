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

#include <surface_mesh/IO.h>

//=============================================================================

namespace surface_mesh {

//=============================================================================

MeshViewer::MeshViewer(const char* title, int width, int height)
    : GlfwViewer(title, width, height)
{
    // setup draw modes
    clearDrawModes();
    addDrawMode("Points");
    addDrawMode("Hidden Line");
    addDrawMode("Flat Shading");
    addDrawMode("Smooth Shading");
    addDrawMode("Vertex Curvature");
    setDrawMode("Flat Shading");

    // initialize curvature texture
    glGenTextures(1, &m_curvatureTexture);
    glBindTexture(GL_TEXTURE_1D, m_curvatureTexture);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, mean_curvature_texture);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    m_nVertices = m_nFaces = 0;
}

//-----------------------------------------------------------------------------

MeshViewer::~MeshViewer()
{
    glDeleteTextures(1, &m_curvatureTexture);
}

//-----------------------------------------------------------------------------

bool MeshViewer::loadMesh(const char* filename)
{
    // load mesh
    if (m_mesh.read(filename))
    {
        auto points = m_mesh.vertexProperty<Point>("v:point");

        // compute bounding box
        Point bbmin(FLT_MAX, FLT_MAX, FLT_MAX);
        Point bbmax(-FLT_MAX, -FLT_MAX, -FLT_MAX);
        for (auto v : m_mesh.vertices())
        {
            bbmin.minimize(points[v]);
            bbmax.maximize(points[v]);
        }

        // update scene center and bounds
        Point  center = (bbmin + bbmax) * 0.5f;
        Scalar radius = norm(bbmax - bbmin) * 0.5f;
        setScene(center, radius);

        // compute face & vertex normals, update face indices
        updateMesh();

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

    // update edge indices for OpenGL rendering
    m_edges.clear();
    m_edges.reserve(m_mesh.nEdges());
    for (auto e : m_mesh.edges())
    {
        m_edges.push_back(m_mesh.vertex(e, 0).idx());
        m_edges.push_back(m_mesh.vertex(e, 1).idx());
    }

    // update face indices for OpenGL rendering
    m_triangles.clear();
    SurfaceMesh::VertexAroundFaceCirculator fvit, fvend;
    SurfaceMesh::Vertex                     v0, v1, v2;
    for (auto f : m_mesh.faces())
    {
        fvit = fvend = m_mesh.vertices(f);
        v0           = *fvit;
        ++fvit;
        v2 = *fvit;
        do
        {
            v1 = v2;
            v2 = *fvit;
            m_triangles.push_back(v0.idx());
            m_triangles.push_back(v1.idx());
            m_triangles.push_back(v2.idx());
        } while (++fvit != fvend);
    }

    m_nVertices = m_mesh.nVertices();
    m_nFaces    = m_mesh.nFaces();
}

//-----------------------------------------------------------------------------

void MeshViewer::draw(const std::string& drawMode)
{
    if (m_mesh.isEmpty())
        return;

    // get required vertex and face properties
    auto points   = m_mesh.vertexProperty<Point>("v:point");
    auto vnormals = m_mesh.vertexProperty<Point>("v:normal");
    auto fnormals = m_mesh.faceProperty<Point>("f:normal");
    auto vcolors  = m_mesh.vertexProperty<Color>("v:color");
    auto vtex1D   = m_mesh.vertexProperty<float>("v:tex1D", 0.0);

    // setup vertex arrays
    glVertexPointer(points.data());
    glNormalPointer(vnormals.data());
    glColorPointer(vcolors.data());
    glTexCoordPointer(vtex1D.data());

    if (drawMode == "Points")
    {
        glEnableClientState(GL_VERTEX_ARRAY);
        glDisable(GL_LIGHTING);
        glColor3f(0.0, 0.0, 0.0);
        glDrawArrays(GL_POINTS, 0, m_nVertices);
        glDisableClientState(GL_VERTEX_ARRAY);
    }

    else if (drawMode == "Hidden Line")
    {
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);

        // draw faces
        glEnable(GL_LIGHTING);
        glShadeModel(GL_SMOOTH);
        glDepthRange(0.01, 1.0);
        glDrawElements(GL_TRIANGLES, (GLsizei)m_triangles.size(),
                       GL_UNSIGNED_INT, &m_triangles[0]);

        // overlay edges
        glVertexPointer(points.data());
        glDisable(GL_LIGHTING);
        glColor3f(0.0, 0.0, 0.0);
        glDepthRange(0.0, 1.0);
        glDepthFunc(GL_LEQUAL);
        glDrawElements(GL_LINES, (GLsizei)m_edges.size(), GL_UNSIGNED_INT,
                       &m_edges[0]);
        glDepthFunc(GL_LESS);

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
    }

    else if (drawMode == "Flat Shading")
    {
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);

        glEnable(GL_LIGHTING);
        glShadeModel(GL_FLAT);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDepthRange(0.01, 1.0);

        for (auto f : m_mesh.faces())
        {
            glBegin(GL_POLYGON);
            glNormal(fnormals[f]);
            for (auto fv : m_mesh.vertices(f))
                glVertex(points[fv]);
            glEnd();
        }
    }

    else if (drawMode == "Smooth Shading")
    {
        glEnable(GL_LIGHTING);
        glShadeModel(GL_SMOOTH);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        glDrawElements(GL_TRIANGLES, (GLsizei)m_triangles.size(),
                       GL_UNSIGNED_INT, &m_triangles[0]);
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
    }

    else if (drawMode == "Vertex Color")
    {
        glDisable(GL_LIGHTING);
        glShadeModel(GL_SMOOTH);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glDrawElements(GL_TRIANGLES, (GLsizei)m_triangles.size(),
                       GL_UNSIGNED_INT, &m_triangles[0]);
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
    }

    else if (drawMode == "Vertex Curvature")
    {
        glBindTexture(GL_TEXTURE_1D, m_curvatureTexture);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        glMatrixMode(GL_TEXTURE);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);

        glEnable(GL_LIGHTING);
        glShadeModel(GL_SMOOTH);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glEnable(GL_TEXTURE_1D);
        glDrawElements(GL_TRIANGLES, (GLsizei)m_triangles.size(),
                       GL_UNSIGNED_INT, &m_triangles[0]);
        glDisable(GL_TEXTURE_1D);
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
}


void MeshViewer::keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS) // only react on key press events
        return;

    GlfwViewer::keyboard(window,key,scancode,action,mods);
}

//=============================================================================
} // namespace surface_mesh
//=============================================================================
