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

#include <pmp/gl/PointSetGL.h>
#include <pmp/gl/phong_shader.h>
#include <float.h>

//=============================================================================

namespace pmp {

//=============================================================================

PointSetGL::PointSetGL()
{
    // initialize GL buffers to zero
    vertex_array_object_ = 0;
    vertex_buffer_       = 0;
    normal_buffer_       = 0;

    // initialize buffer sizes
    n_vertices_ = 0;
}

//-----------------------------------------------------------------------------

PointSetGL::~PointSetGL()
{
    // delete OpenGL buffers
    glDeleteBuffers(1, &vertex_buffer_);
    glDeleteBuffers(1, &normal_buffer_);
    glDeleteVertexArrays(1, &vertex_array_object_);
}

//-----------------------------------------------------------------------------

void PointSetGL::updateOpenGLBuffers()
{
    // are buffers already initialized?
    if (!vertex_array_object_)
    {
        glGenVertexArrays(1, &vertex_array_object_);
        glBindVertexArray(vertex_array_object_);
        glGenBuffers(1, &vertex_buffer_);
        glGenBuffers(1, &normal_buffer_);
    }

    // activate VAO
    glBindVertexArray(vertex_array_object_);

    // vertices
    auto positions = getVertexProperty<Point>("v:point");
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
    glBufferData(GL_ARRAY_BUFFER, nVertices() * 3 * sizeof(float),
                 positions.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    n_vertices_ = nVertices();

    // normals
    auto normals = getVertexProperty<Normal>("v:normal");
    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_);
    glBufferData(GL_ARRAY_BUFFER, nVertices() * 3 * sizeof(float),
                 normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    // unbind vertex array
    glBindVertexArray(0);
}

//-----------------------------------------------------------------------------

void PointSetGL::draw(const mat4& projectionMatrix, const mat4& modelviewMatrix,
                      const std::string drawMode)
{
    // did we generate buffers already?
    if (!vertex_array_object_)
    {
        updateOpenGLBuffers();
    }

    // load shader?
    if (!m_phongShader.isValid())
    {
        m_phongShader.source(phong_vshader, phong_fshader);
        m_phongShader.use();
        m_phongShader.bind_attrib("v_position", 0);
        m_phongShader.bind_attrib("v_normal",   1);
    }

    // empty point set?
    if (isEmpty())
        return;

    // setup matrices
    mat4 mv_matrix  = modelviewMatrix;
    mat4 mvp_matrix = projectionMatrix * modelviewMatrix;
    mat3 n_matrix   = inverse(transpose(mat3(mv_matrix)));

    // setup shader
    m_phongShader.use();
    m_phongShader.set_uniform("modelview_projection_matrix", mvp_matrix);
    m_phongShader.set_uniform("modelview_matrix", mv_matrix);
    m_phongShader.set_uniform("normal_matrix", n_matrix);
    m_phongShader.set_uniform("light1", vec3(1.0, 1.0, 1.0));
    m_phongShader.set_uniform("light2", vec3(-1.0, 1.0, 1.0));
    m_phongShader.set_uniform("front_color", vec3(0.6, 0.6, 0.6));
    m_phongShader.set_uniform("back_color", vec3(0.3, 0.0, 0.0));
    m_phongShader.set_uniform("use_lighting", true);

    glBindVertexArray(vertex_array_object_);

    if (drawMode == "Points")
    {
// draw points
#ifndef __EMSCRIPTEN__
        glPointSize(5.0);
#endif
        glDrawArrays(GL_POINTS, 0, n_vertices_);
    }

    glBindVertexArray(0);
    glCheckError();
}

//=============================================================================
} // namespace pmp
//=============================================================================
