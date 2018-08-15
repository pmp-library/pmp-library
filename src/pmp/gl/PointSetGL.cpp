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

#include <pmp/gl/PointSetGL.h>
#include <cfloat>

static const char* points_vshader =
#ifndef __EMSCRIPTEN__
    "#version 330\n"
#else
    "#version 300 es\n"
#endif
    "\n"
    "layout (location=0) in vec4 v_position;\n"
    "layout (location=1) in vec3 v_normal;\n"
    "layout (location=2) in vec3 v_color;\n"
    "out vec3 v2f_normal;\n"
    "out vec3 v2f_color;\n"
    "out vec3 v2f_view;\n"
    "uniform mat4   modelview_projection_matrix;\n"
    "uniform mat4   modelview_matrix;\n"
    "uniform mat3   normal_matrix;\n"
    "uniform float  point_size;\n"
    "\n"
    "void main()\n"
    "{\n"
    "   v2f_normal   = normal_matrix * v_normal;\n"
    "   v2f_color    = v_color;\n"
    "   v2f_view     = -(modelview_matrix * v_position).xyz;\n"
    "   gl_PointSize = point_size;\n"
    "   gl_Position  = modelview_projection_matrix * v_position;\n"
    "} \n";

static const char* points_fshader =
#ifndef __EMSCRIPTEN__
    "#version 330\n"
#else
    "#version 300 es\n"
    "precision mediump float;\n"
#endif
    "\n"
    "in vec3  v2f_normal;\n"
    "in vec3  v2f_color;\n"
    "in vec3  v2f_view;\n"
    "\n"
    "uniform float  ambient;\n"
    "uniform float  diffuse;\n"
    "uniform float  specular;\n"
    "uniform float  shininess;\n"
    "uniform vec3   light1;\n"
    "uniform vec3   light2;\n"
    "\n"
    "out vec4 f_color;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    vec3 color = gl_FrontFacing ? v2f_color : 0.5*v2f_color;\n"
    "    vec3 rgb;\n"
    "\n"
    "    vec3 L1 = normalize(light1);\n"
    "    vec3 L2 = normalize(light2);\n"
    "    vec3 N  = normalize(v2f_normal);\n"
    "    vec3 V  = normalize(v2f_view);\n"
    "    \n"
    "    if (!gl_FrontFacing) N = -N;\n"
    "    \n"
    "    vec3  R;\n"
    "    float NL, RV;\n"
    "    \n"
    "    rgb = ambient * 0.1 * color;\n"
    "    \n"
    "    NL = dot(N, L1);\n"
    "    if (NL > 0.0)\n"
    "    {\n"
    "        rgb += diffuse * NL * color;\n"
    "        R  = normalize(-reflect(L1, N));\n"
    "        RV = dot(R, V);\n"
    "        if (RV > 0.0) \n"
    "        {\n"
    "            rgb += vec3( specular * pow(RV, shininess) );\n"
    "        }\n"
    "    }\n"
    "    \n"
    "    NL = dot(N, L2);\n"
    "    if (NL > 0.0)\n"
    "    {\n"
    "         rgb += diffuse * NL * color;\n"
    "         R  = normalize(-reflect(L2, N));\n"
    "         RV = dot(R, V);\n"
    "         if (RV > 0.0) \n"
    "         {\n"
    "             rgb += vec3( specular * pow(RV, shininess) );\n"
    "         }\n"
    "    }\n"
    "    \n"
    "    f_color = vec4(rgb, 1.0);\n"
    "}";

//=============================================================================

namespace pmp {

//=============================================================================

PointSetGL::PointSetGL()
{
    // initialize GL buffers to zero
    m_vertexArrayObject = 0;
    m_vertexBuffer = 0;
    m_normalBuffer = 0;
    m_colorBuffer = 0;

    // initialize buffer sizes
    m_nVertices = 0;

    // material parameters
    m_frontColor = vec3(0.6, 0.6, 0.6);
    m_backColor = vec3(0.5, 0.0, 0.0);
    m_ambient = 0.1;
    m_diffuse = 0.8;
    m_specular = 0.6;
    m_shininess = 100.0;
}

//-----------------------------------------------------------------------------

PointSetGL::~PointSetGL()
{
    // delete OpenGL buffers
    glDeleteBuffers(1, &m_vertexBuffer);
    glDeleteBuffers(1, &m_normalBuffer);
    glDeleteBuffers(1, &m_colorBuffer);
    glDeleteVertexArrays(1, &m_vertexArrayObject);
}

//-----------------------------------------------------------------------------

void PointSetGL::updateOpenGLBuffers()
{
    // are buffers already initialized?
    if (!m_vertexArrayObject)
    {
        glGenVertexArrays(1, &m_vertexArrayObject);
        glBindVertexArray(m_vertexArrayObject);
        glGenBuffers(1, &m_vertexBuffer);
        glGenBuffers(1, &m_normalBuffer);
        glGenBuffers(1, &m_colorBuffer);
    }

    // activate VAO
    glBindVertexArray(m_vertexArrayObject);

    // vertices
    auto positions = getVertexProperty<Point>("v:point");
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, nVertices() * 3 * sizeof(float),
                 positions.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);
    m_nVertices = nVertices();

    // normals
    auto normals = getVertexProperty<Normal>("v:normal");
    glBindBuffer(GL_ARRAY_BUFFER, m_normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, nVertices() * 3 * sizeof(float),
                 normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    // colors
    auto colors = getVertexProperty<Color>("v:color");
    if (colors)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_colorBuffer);
        glBufferData(GL_ARRAY_BUFFER, nVertices() * 3 * sizeof(float),
                     colors.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(2);
    }
    else
    {
        glDisableVertexAttribArray(2);
    }

    // unbind vertex array
    glBindVertexArray(0);
}

//-----------------------------------------------------------------------------

void PointSetGL::draw(const mat4& projectionMatrix, const mat4& modelviewMatrix,
                      const std::string drawMode)
{
    // did we generate buffers already?
    if (!m_vertexArrayObject)
    {
        updateOpenGLBuffers();
    }

    // load shader?
    if (!m_phongShader.isValid())
    {
        if (!m_phongShader.source(points_vshader, points_fshader))
            exit(1);
    }

    // empty point set?
    if (isEmpty())
        return;

    // setup matrices
    mat4 mv_matrix = modelviewMatrix;
    mat4 mvp_matrix = projectionMatrix * modelviewMatrix;
    mat3 n_matrix = inverse(transpose(linearPart(mv_matrix)));

    // setup shader
    m_phongShader.use();
    m_phongShader.setUniform("modelview_projection_matrix", mvp_matrix);
    m_phongShader.setUniform("modelview_matrix", mv_matrix);
    m_phongShader.setUniform("normal_matrix", n_matrix);
    m_phongShader.setUniform("point_size", 5.0f);
    m_phongShader.setUniform("light1", vec3(1.0, 1.0, 1.0));
    m_phongShader.setUniform("light2", vec3(-1.0, 1.0, 1.0));
    m_phongShader.setUniform("ambient", m_ambient);
    m_phongShader.setUniform("diffuse", m_diffuse);
    m_phongShader.setUniform("specular", m_specular);
    m_phongShader.setUniform("shininess", m_shininess);

    // per-vertex color or per-object color?
    if (!getVertexProperty<Color>("v:color"))
    {
        glVertexAttrib3fv(2, m_frontColor.data());
    }

    glBindVertexArray(m_vertexArrayObject);

    if (drawMode == "Points")
    {
        glEnable(GL_PROGRAM_POINT_SIZE);
        glDrawArrays(GL_POINTS, 0, m_nVertices);
    }

    glBindVertexArray(0);
    glCheckError();
}

//=============================================================================
} // namespace pmp
//=============================================================================
