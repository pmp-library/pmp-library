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

#include "GlfwViewer.h"
#include <algorithm>
//=============================================================================

namespace surface_mesh {

//=============================================================================

GlfwViewer* GlfwViewer::m_instance = nullptr;

//-----------------------------------------------------------------------------

GlfwViewer::GlfwViewer(const char* title, int width, int height)
    : m_width(width), m_height(height)
{
    // initialize glfw window
    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_SAMPLES, 4); // request samples for multi-sampling

    m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);

    //glEnable(GL_MULTISAMPLE);

    if (!m_window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);

    m_instance = this;

    // detect highDPI scaling
    int windowWidth, windowHeight, framebufferWidth, framebufferHeight;
    glfwGetWindowSize(m_window, &windowWidth, &windowHeight);
    glfwGetFramebufferSize(m_window, &framebufferWidth, &framebufferHeight);
    m_scaling = framebufferWidth / windowWidth;
    m_width   = framebufferWidth;
    m_height  = framebufferHeight;
    if (m_scaling != 1) std::cout << "highDPI scaling: " << m_scaling << std::endl;

    // register glfw callbacks
    glfwSetKeyCallback(m_window, glfwKeyboard);
    glfwSetCursorPosCallback(m_window, glfwMotion);
    glfwSetMouseButtonCallback(m_window, glfwMouse);
    glfwSetScrollCallback(m_window, glfwScroll);
    glfwSetFramebufferSizeCallback(m_window, glfwReshape);

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

GlfwViewer::~GlfwViewer()
{
    // terminate GLFW
    glfwTerminate();
}

//-----------------------------------------------------------------------------

int GlfwViewer::run()
{
    while (!glfwWindowShouldClose(m_window))
    {
        // draw scene
        display();

        // swap buffers
        glfwSwapBuffers(m_window);

        // handle events
        glfwPollEvents();
    }

    glfwDestroyWindow(m_window);

    return EXIT_SUCCESS;
}

//-----------------------------------------------------------------------------

void GlfwViewer::clearDrawModes()
{
    m_nDrawModes = 0;
    m_drawModeNames.clear();
}

//-----------------------------------------------------------------------------

unsigned int GlfwViewer::addDrawMode(const std::string& _s)
{
    ++m_nDrawModes;
    m_drawModeNames.push_back(_s);

    return m_nDrawModes - 1;
}

//-----------------------------------------------------------------------------

void GlfwViewer::setDrawMode(const std::string& _s)
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

void GlfwViewer::glfwError(int error, const char* description)
{
    std::cerr << "error (" << error << "):" << description << std::endl;
}

//-----------------------------------------------------------------------------

void GlfwViewer::glfwKeyboard(GLFWwindow* window, int key, int scancode,
                              int action, int mods)
{
    m_instance->keyboard(window, key, scancode, action, mods);
}

//-----------------------------------------------------------------------------

void GlfwViewer::glfwMotion(GLFWwindow* window, double xpos, double ypos)
{
    m_instance->motion(window, m_instance->m_scaling*xpos, m_instance->m_scaling*ypos);
}

//-----------------------------------------------------------------------------

void GlfwViewer::glfwMouse(GLFWwindow* window, int button, int action, int mods)
{
    m_instance->mouse(window, button, action, mods);
}

//-----------------------------------------------------------------------------

void GlfwViewer::glfwScroll(GLFWwindow* window, double xoffset, double yoffset)
{
    m_instance->scroll(window, xoffset, yoffset);
}

//-----------------------------------------------------------------------------

void GlfwViewer::glfwReshape(GLFWwindow* window, int width, int height)
{
    m_instance->reshape(window, width, height);
}

//-----------------------------------------------------------------------------

void GlfwViewer::keyboard(GLFWwindow* /*window*/, int key, int /*scancode*/,
                          int action, int /*mods*/)
{
    if (action != GLFW_PRESS) // only react on key press events
        return;

    switch (key)
    {
        case GLFW_KEY_ESCAPE:
        case GLFW_KEY_Q:
        {
            exit(0);
            break;
        }
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

void GlfwViewer::reshape(GLFWwindow* /*window*/, int width, int height)
{
    m_width  = width;
    m_height = height;

    glViewport(0, 0, width, height);
}

//-----------------------------------------------------------------------------

void GlfwViewer::display(void)
{
    // clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // adjust clipping planes to tightly fit bounding sphere
    float z = -((m_modelviewMatrix[2] * m_center[0] +
                 m_modelviewMatrix[6] * m_center[1] +
                 m_modelviewMatrix[10] * m_center[2] + m_modelviewMatrix[14]) /
                (m_modelviewMatrix[3] * m_center[0] +
                 m_modelviewMatrix[7] * m_center[1] +
                 m_modelviewMatrix[11] * m_center[2] + m_modelviewMatrix[15]));
    m_near = 0.01 * m_radius;
    m_far  = 10.0 * m_radius;
    m_fovy = 45.0;
    m_near = std::max(0.001f * m_radius, z - m_radius);
    m_far  = std::max(0.002f * m_radius, z + m_radius);

    // update projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(m_fovy, (GLfloat)m_width / (GLfloat)m_height, m_near, m_far);
    glGetDoublev(GL_PROJECTION_MATRIX, m_projectionMatrix);
    glMatrixMode(GL_MODELVIEW);

    // draw the scene in current draw mode
    if (m_drawMode < m_drawModeNames.size())
        draw(m_drawModeNames[m_drawMode]);
    else
        draw("");
}

//-----------------------------------------------------------------------------

void GlfwViewer::mouse(GLFWwindow* /*window*/, int button, int action, int mods)
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

void GlfwViewer::scroll(GLFWwindow* /*window*/, double /*xoffset*/,
                        double yoffset)
{
    m_wheelPos += yoffset;
    float d = -(float)yoffset * 0.12 * m_radius;
    translate(Vec3f(0.0, 0.0, d));
}

//-----------------------------------------------------------------------------

void GlfwViewer::motion(GLFWwindow* /*window*/, double xpos, double ypos)
{
    std::cerr << xpos << ", " << ypos << std::endl;
    std::cerr << m_width << ", " << m_height << std::endl;


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
    m_mousePos2D  = Vec2f(xpos, ypos);
    m_lastPoint2D = Vec2i(xpos, ypos);
    m_lastPointOk = mapToSphere(m_lastPoint2D, m_lastPoint3D);
}

//-----------------------------------------------------------------------------

void GlfwViewer::init()
{
    // OpenGL state
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glColor4f(0.0, 0.0, 0.0, 1.0);
    glDisable(GL_DITHER);
    glEnable(GL_DEPTH_TEST);

    // some performance settings
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);

    // material
    GLfloat mat_a[] = {0.2, 0.2, 0.2, 1.0};
    GLfloat mat_d[] = {0.5, 0.525, 0.6, 1.0};
    GLfloat mat_s[] = {0.6, 0.65, 0.7, 1.0};
    GLfloat shine[] = {128.0};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_a);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_d);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_s);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shine);

    // lighting
    glLoadIdentity();

    GLfloat pos1[] = {0.1, 0.1, -0.02, 0.0};
    GLfloat pos2[] = {-0.1, 0.1, -0.02, 0.0};
    GLfloat pos3[] = {0.0, 0.0, 0.1, 0.0};
    GLfloat col1[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat col2[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat col3[] = {1.0, 1.0, 1.0, 1.0};

    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, pos1);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, col1);
    glLightfv(GL_LIGHT0, GL_SPECULAR, col1);

    glEnable(GL_LIGHT1);
    glLightfv(GL_LIGHT1, GL_POSITION, pos2);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, col2);
    glLightfv(GL_LIGHT1, GL_SPECULAR, col2);

    glEnable(GL_LIGHT2);
    glLightfv(GL_LIGHT2, GL_POSITION, pos3);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, col3);
    glLightfv(GL_LIGHT2, GL_SPECULAR, col3);

    // scene pos and size
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glGetDoublev(GL_MODELVIEW_MATRIX, m_modelviewMatrix);
    setScene(Vec3f(0.0, 0.0, 0.0), 1.0);
}

//-----------------------------------------------------------------------------

void GlfwViewer::draw(const std::string& drawMode)
{
    if (drawMode == "Wireframe")
    {
        glDisable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else if (drawMode == "Solid Flat")
    {
        glEnable(GL_LIGHTING);
        glShadeModel(GL_FLAT);
    }
    else if (drawMode == "Solid Smooth")
    {
        glEnable(GL_LIGHTING);
        glShadeModel(GL_SMOOTH);
    }
}

//-----------------------------------------------------------------------------

void GlfwViewer::setScene(const Vec3f& center, float radius)
{
    m_center = center;
    m_radius = radius;
    viewAll();
}

//-----------------------------------------------------------------------------

void GlfwViewer::viewAll()
{
    translate(
        Vec3f(-(m_modelviewMatrix[0] * m_center[0] +
                m_modelviewMatrix[4] * m_center[1] +
                m_modelviewMatrix[8] * m_center[2] + m_modelviewMatrix[12]),
              -(m_modelviewMatrix[1] * m_center[0] +
                m_modelviewMatrix[5] * m_center[1] +
                m_modelviewMatrix[9] * m_center[2] + m_modelviewMatrix[13]),
              -(m_modelviewMatrix[2] * m_center[0] +
                m_modelviewMatrix[6] * m_center[1] +
                m_modelviewMatrix[10] * m_center[2] + m_modelviewMatrix[14] +
                3.0 * m_radius)));
}

//-----------------------------------------------------------------------------

bool GlfwViewer::mapToSphere(const Vec2i& point2D, Vec3f& result)
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

void GlfwViewer::rotation(int x, int y)
{
    if (m_lastPointOk)
    {
        Vec2i newPoint2D;
        Vec3f newPoint3D;
        bool  newPointok;

        newPoint2D = Vec2i(x, y);
        newPointok = mapToSphere(newPoint2D, newPoint3D);

        if (newPointok)
        {
            Vec3f axis     = cross(m_lastPoint3D, newPoint3D);
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

void GlfwViewer::translation(int x, int y)
{
    float dx = x - m_lastPoint2D[0];
    float dy = y - m_lastPoint2D[1];

    float z = -((m_modelviewMatrix[2] * m_center[0] +
                 m_modelviewMatrix[6] * m_center[1] +
                 m_modelviewMatrix[10] * m_center[2] + m_modelviewMatrix[14]) /
                (m_modelviewMatrix[3] * m_center[0] +
                 m_modelviewMatrix[7] * m_center[1] +
                 m_modelviewMatrix[11] * m_center[2] + m_modelviewMatrix[15]));

    float aspect = (float)m_width / (float)m_height;
    float up     = tan(m_fovy / 2.0f * M_PI / 180.f) * m_near;
    float right  = aspect * up;

    translate(Vec3f(2.0 * dx / m_width * right / m_near * z,
                    -2.0 * dy / m_height * up / m_near * z, 0.0f));
}

//-----------------------------------------------------------------------------

void GlfwViewer::zoom(int, int y)
{
    float dy = y - m_lastPoint2D[1];
    float h  = m_height;
    translate(Vec3f(0.0, 0.0, m_radius * dy * 3.0 / h));
}

//-----------------------------------------------------------------------------

void GlfwViewer::translate(const Vec3f& trans)
{
    glLoadIdentity();
    glTranslated(trans[0], trans[1], trans[2]);
    glMultMatrixd(m_modelviewMatrix);
    glGetDoublev(GL_MODELVIEW_MATRIX, m_modelviewMatrix);
}

//-----------------------------------------------------------------------------

void GlfwViewer::rotate(const Vec3f& axis, float angle)
{
    Vec3f t(m_modelviewMatrix[0] * m_center[0] +
                m_modelviewMatrix[4] * m_center[1] +
                m_modelviewMatrix[8] * m_center[2] + m_modelviewMatrix[12],
            m_modelviewMatrix[1] * m_center[0] +
                m_modelviewMatrix[5] * m_center[1] +
                m_modelviewMatrix[9] * m_center[2] + m_modelviewMatrix[13],
            m_modelviewMatrix[2] * m_center[0] +
                m_modelviewMatrix[6] * m_center[1] +
                m_modelviewMatrix[10] * m_center[2] + m_modelviewMatrix[14]);

    glLoadIdentity();
    glTranslatef(t[0], t[1], t[2]);
    glRotated(angle, axis[0], axis[1], axis[2]);
    glTranslatef(-t[0], -t[1], -t[2]);
    glMultMatrixd(m_modelviewMatrix);
    glGetDoublev(GL_MODELVIEW_MATRIX, m_modelviewMatrix);
}

//=============================================================================
} // namespace surface_mesh
//=============================================================================
