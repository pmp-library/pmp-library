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
#pragma once
//=============================================================================

#include <surface_mesh/Vector.h>
#include <surface_mesh/gl/glwrappers.h>

#include <GLFW/glfw3.h>

#include <map>
#include <string>
#include <vector>

//=============================================================================

namespace surface_mesh {

//=============================================================================

//! \addtogroup gl gl
//! @{

//! A simple GLFW viewer with trackball user interface
class GlfwViewer
{

public: //------------------------------------------------------ public methods
    //! constructor
    GlfwViewer(const char* title, int width = 0, int height = 0);

    //! destructor
    virtual ~GlfwViewer();

    //! main window loop
    int run();

    //! define the center and radius of the scene/
    //! used for trackball rotation
    void setScene(const Vec3f& center, float radius);

    //! adjust camera such that the whole scene (defined by set_scene()) is visible
    void viewAll();

private: //----------------------------- static wrapper functions for callbacks
    static void glfwError(int error, const char* description);
    static void glfwKeyboard(GLFWwindow* window, int key, int scancode,
                             int action, int mods);
    static void glfwMouse(GLFWwindow* window, int button, int action, int mods);
    static void glfwMotion(GLFWwindow* window, double xpos, double ypos);
    static void glfwScroll(GLFWwindow* window, double xoffset, double yoffset);
    static void glfwReshape(GLFWwindow* window, int width, int height);

    static GlfwViewer* m_instance;

protected: //----------------------------------- callbacks as member functions
    //! this function is called when the scene has to be rendered. it
    //! clears the buffers, calls the draw() method, and performs buffer swap
    virtual void display(void);

    //! this function handles keyboard events
    virtual void keyboard(GLFWwindow* window, int key, int scancode, int action,
                          int mods);

    //! this function handles mouse button events
    virtual void mouse(GLFWwindow* window, int button, int action, int mods);

    //! this function handles mouse motion (passive/active position)
    virtual void motion(GLFWwindow* window, double xpos, double ypos);

    //! this function handles mouse scroll events
    virtual void scroll(GLFWwindow* window, double xoffset, double yoffset);

    //! this function is called if the window is resized
    virtual void reshape(GLFWwindow* window, int width, int height);

protected: //------------------------------------------- handling of draw modes
    //! reset the list of draw modes
    void clearDrawModes();

    //! add a draw mode
    unsigned int addDrawMode(const std::string& drawMode);

    //! activate a draw mode
    void setDrawMode(const std::string& drawMode);

protected: //-------------------------------------------------------- rendering
    //! initialize all OpenGL states
    virtual void init();

    //! this function is responsible for rendering the scene
    virtual void draw(const std::string& drawMode);

protected: //-------------------------------------------- trackball interaction
    //! turn a mouse event into a rotation around the scene center. calls rotate().
    void rotation(int x, int y);

    //! turn a mouse event into a translation in the view plane. calls translate().
    void translation(int x, int y);

    //! turn a mouse event into a zoom, i.e., translation in z-direction. calls translate().
    void zoom(int x, int y);

    //! translate the scene and update modelview matrix
    void translate(const Vec3f& trans);

    //! rotate the scene (around its center) and update modelview matrix
    void rotate(const Vec3f& axis, float angle);

    //! virtual trackball: map 2D screen point to unit sphere. used by rotate().
    bool mapToSphere(const Vec2i& point, Vec3f& result);

protected: //----------------------------------------------------- private data
    //! GLFW window pointer
    GLFWwindow* m_window;

    //! current viewport dimension
    int m_width, m_height;

    //! draw modes
    unsigned int             m_drawMode;
    unsigned int             m_nDrawModes;
    std::vector<std::string> m_drawModeNames;

    //! scene position and dimension
    Vec3f m_center;
    float m_radius;

    //! projection parameters
    float m_near, m_far, m_fovy;

    //! OpenGL matrices
    double m_projectionMatrix[16];
    double m_modelviewMatrix[16];

    //! trackball helpers
    Vec2f m_mousePos2D;
    Vec2i m_lastPoint2D;
    Vec3f m_lastPoint3D;
    bool  m_lastPointOk;
    bool  m_buttonDown[7];
    int   m_modifiers;
    int   m_wheelPos;
};

//! @}

//=============================================================================
} // namespace surface_mesh
//=============================================================================
