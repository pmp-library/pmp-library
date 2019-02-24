//=============================================================================
// Copyright (C) 2011-2019 The pmp-library developers
//
// This file is part of the Polygon Mesh Processing Library.
// Distributed under the terms of the MIT license, see LICENSE.txt for details.
//
// SPDX-License-Identifier: MIT
//=============================================================================
#pragma once
//=============================================================================

#include <pmp/visualization/GL.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <utility>

//=============================================================================

namespace pmp {

//=============================================================================

//! \addtogroup visualization visualization
//! @{

//! A window provided by GLFW
class Window
{

public: //------------------------------------------------------ public methods
    //! constructor
    Window(const char* title, int width, int height, bool showgui = true);

    //! destructor
    virtual ~Window();

    //! main window loop
    int run();

private: //----------------------------- static wrapper functions for callbacks
    static void glfw_error(int error, const char* description);
    static void glfw_keyboard(GLFWwindow* window, int key, int scancode,
                              int action, int mods);
    static void glfw_character(GLFWwindow* window, unsigned int c);
    static void glfw_mouse(GLFWwindow* window, int button, int action,
                           int mods);
    static void glfw_motion(GLFWwindow* window, double xpos, double ypos);
    static void glfw_scroll(GLFWwindow* window, double xoffset, double yoffset);
    static void glfw_resize(GLFWwindow* window, int width, int height);

    static void render_frame();

    static Window* instance_;

protected: //----------------------------------- callbacks as member functions
    //! this function is called when the scene has to be rendered. it
    //! clears the buffers, calls the draw() method, and performs buffer swap
    virtual void display(void) = 0;

    //! this function handles keyboard events
    virtual void keyboard(int /*key*/, int /*code*/, int /*action*/,
                          int /*mods*/);

    //! this function handles unicode character events
    virtual void character(unsigned int);

    //! this function handles mouse button events
    virtual void mouse(int /*button*/, int /*action*/, int /*mods*/) {}

    //! this function handles mouse motion (passive/active position)
    virtual void motion(double /*xpos*/, double /*ypos*/) {}

    //! this function handles mouse scroll events
    virtual void scroll(double /*xoffset*/, double /*yoffset*/) {}

    //! this function is called if the window is resized
    virtual void resize(int /*width*/, int /*height*/) {}

    //! this function renders the ImGUI elements and handles their events
    virtual void process_imgui() {}

    //! this function is called just before rendering
    virtual void do_processing() {}

    //! get position of mouse cursor
    void cursor_pos(double& x, double& y) const;

protected:
    //! setup ImGUI user interface
    void init_imgui();

    //! scale ImGUI elements and font
    void scale_imgui(float scale);

    //! is ImGUI visible or hidden?
    bool show_imgui() const { return show_imgui_; }

    //!  show or hide ImGUI
    void show_imgui(bool b) { show_imgui_ = b; }

    //! add key binding (or general action description)
    void add_help_item(std::string key, std::string description);

    //! show ImGUI help dialog
    void show_help();

protected:
    //! GLFW window pointer
    GLFWwindow* window_;

    //! current viewport dimension
    int width_, height_;

    //! highDPI scaling
    float scaling_, pixel_ratio_;

    bool show_imgui_;
    float imgui_scale_;
    bool show_help_;
    std::vector< std::pair<std::string, std::string> > help_items_;
};

//=============================================================================
//! @}
//=============================================================================
} // namespace pmp
//=============================================================================
