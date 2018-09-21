//=============================================================================
// Copyright (C) 2011-2017 The pmp-library developers
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

#include <pmp/gl/Window.h>
#include <pmp/MatVec.h>

#include <string>
#include <vector>

//=============================================================================

namespace pmp {

//=============================================================================

//! \addtogroup gl gl
//! @{

//! A simple GLFW viewer with trackball user interface
class TrackballViewer : public Window
{

public: //------------------------------------------------------ public methods
    //! constructor
    TrackballViewer(const char* title, int width, int height,
                    bool showgui = true);

    //! destructor
    virtual ~TrackballViewer();

    //! define the center and radius of the scene/
    //! used for trackball rotation
    void setScene(const vec3& center, float radius);

    //! adjust camera such that the whole scene (defined by set_scene()) is visible
    void viewAll();

protected: //----------------------------------- callbacks as member functions
    //! this function is called when the scene has to be rendered. it
    //! clears the buffers, calls the draw() method, and performs buffer swap
    virtual void display(void) override;

    //! this function handles keyboard events
    virtual void keyboard(int key, int code, int action, int mods) override;

    //! this function handles mouse button events
    virtual void mouse(int button, int action, int mods) override;

    //! this function handles mouse motion (passive/active position)
    virtual void motion(double xpos, double ypos) override;

    //! this function handles mouse scroll events
    virtual void scroll(double xoffset, double yoffset) override;

    //! this function is called if the window is resized
    virtual void resize(int width, int height) override;

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
    virtual void draw(const std::string& drawMode) = 0;

protected: //-------------------------------------------- trackball interaction
    //! turn a mouse event into a rotation around the scene center. calls rotate().
    void rotation(int x, int y);

    //! turn a mouse event into a translation in the view plane. calls translate().
    void translation(int x, int y);

    //! turn a mouse event into a zoom, i.e., translation in z-direction. calls translate().
    void zoom(int x, int y);

    //! get 3D position under the mouse cursor
    bool pick(int x, int y, vec3& result);

    //! fly toward the position under the mouse cursor and set rotation center to it
    void flyTo(int x, int y);

    //! translate the scene and update modelview matrix
    void translate(const vec3& trans);

    //! rotate the scene (around its center) and update modelview matrix
    void rotate(const vec3& axis, float angle);

    //! virtual trackball: map 2D screen point to unit sphere. used by rotate().
    bool mapToSphere(const ivec2& point, vec3& result);

protected: //----------------------------------------------------- private data
    //! draw modes
    unsigned int m_drawMode;
    unsigned int m_nDrawModes;
    std::vector<std::string> m_drawModeNames;

    //! scene position and dimension
    vec3 m_center;
    float m_radius;

    //! projection parameters
    float m_near, m_far, m_fovy;

    //! OpenGL matrices
    mat4 m_projectionMatrix;
    mat4 m_modelviewMatrix;

    //! trackball helpers
    vec2 m_mousePos2D;
    ivec2 m_lastPoint2D;
    vec3 m_lastPoint3D;
    bool m_lastPointOk;
    bool m_buttonDown[7];
    int m_modifiers;
    int m_wheelPos;
};

//=============================================================================
//! @}
//=============================================================================
} // namespace pmp
//=============================================================================
