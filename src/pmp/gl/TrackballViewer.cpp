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

#include "TrackballViewer.h"
#include <algorithm>

//=============================================================================

namespace pmp {

//=============================================================================

TrackballViewer::TrackballViewer(const char* title, int width, int height)
    : Window(title, width, height)
{
    // init mouse buttons
    for (int i          = 0; i < 7; ++i)
        m_buttonDown[i] = false;
    m_wheelPos          = 0;

    // define basic draw modes
    addDrawMode("Wireframe");
    addDrawMode("Solid Flat");
    addDrawMode("Solid Smooth");
    setDrawMode("Solid Smooth");

    // init OpenGL state
    init();
}

//-----------------------------------------------------------------------------

TrackballViewer::~TrackballViewer()
{
}

//-----------------------------------------------------------------------------

void TrackballViewer::clearDrawModes()
{
    m_nDrawModes = 0;
    m_drawModeNames.clear();
}

//-----------------------------------------------------------------------------

unsigned int TrackballViewer::addDrawMode(const std::string& _s)
{
    ++m_nDrawModes;
    m_drawModeNames.push_back(_s);

    return m_nDrawModes - 1;
}

//-----------------------------------------------------------------------------

void TrackballViewer::setDrawMode(const std::string& _s)
{
    for (unsigned int i = 0; i < m_drawModeNames.size(); ++i)
    {
        if (m_drawModeNames[i] == _s)
        {
            m_drawMode = i;
            break;
        }
    }
}

//-----------------------------------------------------------------------------

void TrackballViewer::keyboard(GLFWwindow* /*window*/, int key, int /*scancode*/,
                              int action, int /*mods*/)
{
    if (action != GLFW_PRESS) // only react on key press events
        return;

    switch (key)
    {
#ifndef __EMSCRIPTEN__
        case GLFW_KEY_ESCAPE:
        case GLFW_KEY_Q:
        {
            exit(0);
            break;
        }
#endif
        case GLFW_KEY_SPACE:
        {
            if (++m_drawMode >= m_nDrawModes)
                m_drawMode   = 0;
            std::string mode = m_drawModeNames[m_drawMode];
            std::cout << "setting draw mode to " << mode << std::endl;
            setDrawMode(mode);
            break;
        }
    }
}

//-----------------------------------------------------------------------------

void TrackballViewer::resize(GLFWwindow* /*window*/, int width, int height)
{
    m_width  = width;
    m_height = height;

    glViewport(0, 0, width, height);
}

//-----------------------------------------------------------------------------

void TrackballViewer::display(void)
{
    // clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // adjust clipping planes to tightly fit bounding sphere
    vec4  mc(m_center, 1.0);
    vec4  ec = m_modelviewMatrix * mc;
    float z = -ec[2];
    m_near = 0.01 * m_radius;
    m_far  = 10.0 * m_radius;
    m_fovy = 45.0;
    m_near = std::max(0.001f * m_radius, z - m_radius);
    m_far  = std::max(0.002f * m_radius, z + m_radius);

    // update projection matrix
    m_projectionMatrix = mat4::perspective(m_fovy, (float)m_width / (float)m_height, m_near, m_far);

    // draw the scene in current draw mode
    if (m_drawMode < m_drawModeNames.size())
        draw(m_drawModeNames[m_drawMode]);
    else
        draw("");
}

//-----------------------------------------------------------------------------

void TrackballViewer::mouse(GLFWwindow* /*window*/, int button, int action, int mods)
{
    // mouse press
    if (action == GLFW_PRESS)
    {
        m_lastPointOk        = mapToSphere(m_lastPoint2D, m_lastPoint3D);
        m_buttonDown[button] = true;
    }

    // mouse release
    else
    {
        m_lastPointOk        = false;
        m_buttonDown[button] = false;
    }

    m_modifiers = mods;
}

//-----------------------------------------------------------------------------

void TrackballViewer::scroll(GLFWwindow* /*window*/, double /*xoffset*/,
                        double yoffset)
{
    m_wheelPos += yoffset;
    float d = -(float)yoffset * 0.12 * m_radius;
#ifdef __EMSCRIPTEN__
    d = -d; // emscripten behavior is the opposite
#endif
    translate(vec3(0.0, 0.0, d));
}

//-----------------------------------------------------------------------------

void TrackballViewer::motion(GLFWwindow* /*window*/, double xpos, double ypos)
{
    // zoom
    if ((m_buttonDown[GLFW_MOUSE_BUTTON_LEFT] &&
         m_buttonDown[GLFW_MOUSE_BUTTON_MIDDLE]) ||
        (m_buttonDown[GLFW_MOUSE_BUTTON_LEFT] &&
         (m_modifiers == GLFW_MOD_SHIFT)))
    {
        zoom(xpos, ypos);
    }

    // translation
    else if (m_buttonDown[GLFW_MOUSE_BUTTON_MIDDLE] ||
             (m_buttonDown[GLFW_MOUSE_BUTTON_LEFT] &&
              (m_modifiers == GLFW_MOD_ALT)))
    {
        translation(xpos, ypos);
    }

    // rotation
    else if (m_buttonDown[GLFW_MOUSE_BUTTON_LEFT])
    {
        rotation(xpos, ypos);
    }

    // remember points
    m_lastPoint2D = ivec2(xpos, ypos);
    m_lastPointOk = mapToSphere(m_lastPoint2D, m_lastPoint3D);
}

//-----------------------------------------------------------------------------

void TrackballViewer::init()
{
    // set initial state
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_DEPTH_TEST);

    // init modelview
    m_modelviewMatrix = mat4::identity();

    // turn on multi-sampling to anti-alias lines
    glEnable(GL_MULTISAMPLE);
    GLint n_samples;
    glGetIntegerv(GL_SAMPLES, &n_samples);
    std::cout << "Multi-sampling uses " << n_samples << " per pixel\n";
}

//-----------------------------------------------------------------------------

void TrackballViewer::setScene(const vec3& center, float radius)
{
    m_center = center;
    m_radius = radius;
    viewAll();
}

//-----------------------------------------------------------------------------

void TrackballViewer::viewAll()
{
    vec4 c = vec4(m_center, 1.0);
    vec4 t = m_modelviewMatrix * c;
    translate( vec3(-t[0], -t[1], -t[2]-3.0*m_radius ) );
}

//-----------------------------------------------------------------------------

bool TrackballViewer::mapToSphere(const ivec2& point2D, vec3& result)
{
    if ((point2D[0] >= 0) && (point2D[0] <= m_width) && (point2D[1] >= 0) &&
        (point2D[1] <= m_height))
    {
        double x    = (double)(point2D[0] - 0.5 * m_width) / (double)m_width;
        double y    = (double)(0.5 * m_height - point2D[1]) / (double)m_height;
        double sinx = sin(M_PI * x * 0.5);
        double siny = sin(M_PI * y * 0.5);
        double sinx2siny2 = sinx * sinx + siny * siny;

        result[0] = sinx;
        result[1] = siny;
        result[2] = sinx2siny2 < 1.0 ? sqrt(1.0 - sinx2siny2) : 0.0;

        return true;
    }
    else
        return false;
}

//-----------------------------------------------------------------------------

void TrackballViewer::rotation(int x, int y)
{
    if (m_lastPointOk)
    {
        ivec2 newPoint2D;
        vec3 newPoint3D;
        bool  newPointok;

        newPoint2D = ivec2(x, y);
        newPointok = mapToSphere(newPoint2D, newPoint3D);

        if (newPointok)
        {
            vec3 axis     = cross(m_lastPoint3D, newPoint3D);
            float cosAngle = dot(m_lastPoint3D, newPoint3D);

            if (fabs(cosAngle) < 1.0)
            {
                float angle = 2.0 * acos(cosAngle) * 180.0 / M_PI;
                rotate(axis, angle);
            }
        }
    }
}

//-----------------------------------------------------------------------------

void TrackballViewer::translation(int x, int y)
{
    float dx = x - m_lastPoint2D[0];
    float dy = y - m_lastPoint2D[1];

    vec4 mc = vec4(m_center, 1.0);
    vec4 ec = m_modelviewMatrix * mc;
    float z = -(ec[2]/ec[3]);

    float aspect = (float)m_width / (float)m_height;
    float up     = tan(m_fovy / 2.0f * M_PI / 180.f) * m_near;
    float right  = aspect * up;

    translate(vec3( 2.0 * dx / m_width * right / m_near * z,
                   -2.0 * dy / m_height * up / m_near * z, 0.0f));
}

//-----------------------------------------------------------------------------

void TrackballViewer::zoom(int, int y)
{
    float dy = y - m_lastPoint2D[1];
    float h  = m_height;
    translate(vec3(0.0, 0.0, m_radius * dy * 3.0 / h));
}

//-----------------------------------------------------------------------------

void TrackballViewer::translate(const vec3& t)
{
    m_modelviewMatrix = mat4::translate(t) * m_modelviewMatrix;
}

//-----------------------------------------------------------------------------

void TrackballViewer::rotate(const vec3& axis, float angle)
{
    // center in eye coordinates
    vec4 mc = vec4(m_center, 1.0);
    vec4 ec = m_modelviewMatrix * mc;
    vec3  c(ec[0]/ec[3], ec[1]/ec[3], ec[2]/ec[3]);

    m_modelviewMatrix = mat4::translate(c) *
                        mat4::rotate(axis, angle) *
                        mat4::translate(-c) *
                        m_modelviewMatrix;
}

//=============================================================================
} // namespace pmp
//=============================================================================
