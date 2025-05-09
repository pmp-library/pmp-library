// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include "pmp/visualization/gl.h"

#include <vector>
#include <utility>
#include <array>
#include <string>

#include <GLFW/glfw3.h>

#if __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#endif

namespace pmp {

//! A window provided by GLFW
//! \ingroup visualization
class Window
{
public:
    //! constructor
    Window(const char* title, int width, int height, bool showgui = true);

    //! destructor
    virtual ~Window();

    //! main window loop
    int run();

protected:
    //! this function is called when the scene has to be rendered. it
    //! clears the buffers, calls the draw() method, and performs buffer swap
    virtual void display() = 0;

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

    //! this function is called if a file is dropped onto the window
    virtual void drop(int /*count*/, const char** /*paths*/) {}

#if __EMSCRIPTEN__
    virtual void touchstart(const EmscriptenTouchEvent*) {}
    virtual void touchmove(const EmscriptenTouchEvent*) {}
    virtual void touchend(const EmscriptenTouchEvent*) {}
#endif

    //! this function renders the ImGUI elements and handles their events
    virtual void process_imgui() {}

    //! this function is called just before rendering
    virtual void do_processing() {}

    //! setup ImGUI user interface
    void init_imgui();

    //! scale ImGUI elements and font
    void scale_imgui(float scale);

    //! is ImGUI visible or hidden?
    bool show_imgui() const { return show_imgui_; }

    //!  show or hide ImGUI
    void show_imgui(bool b) { show_imgui_ = b; }

    //! clear help items
    void clear_help_items();

    //! add key binding (or general action description)
    void add_help_item(std::string key, std::string description,
                       int position = -1);

    //! show ImGUI help dialog
    void show_help();

    //! take a screenshot, save it to `title-n.png` using the window title
    //! and an incremented number `n`.
    void screenshot();

    //! width of window
    int width() const { return width_; }

    //! height of window
    int height() const { return height_; }

    //! highDPI scaling
    float high_dpi_scaling() const { return scaling_; }

    //! imgui scaling
    float imgui_scaling() const { return imgui_scale_; }

    //! get position of mouse cursor
    void cursor_pos(double& x, double& y) const;

    //! is left mouse button pressed down?
    bool left_mouse_pressed() const { return button_[GLFW_MOUSE_BUTTON_LEFT]; }
    //! is right mouse button pressed down?
    bool right_mouse_pressed() const
    {
        return button_[GLFW_MOUSE_BUTTON_RIGHT];
    }
    //! is middle mouse button pressed down?
    bool middle_mouse_pressed() const
    {
        return button_[GLFW_MOUSE_BUTTON_MIDDLE];
    }

    //! is CTRL modifier key pressed down?
    bool ctrl_pressed() const { return ctrl_pressed_; }
    //! is ALT modifier key pressed down?
    bool alt_pressed() const { return alt_pressed_; }
    //! is SHIFT modifier key pressed down?
    bool shift_pressed() const { return shift_pressed_; }

    static void render_frame();

private:
    static void glfw_error(int error, const char* description);
    static void glfw_keyboard(GLFWwindow* window, int key, int scancode,
                              int action, int mods);
    static void glfw_character(GLFWwindow* window, unsigned int c);
    static void glfw_mouse(GLFWwindow* window, int button, int action,
                           int mods);
    static void glfw_motion(GLFWwindow* window, double xpos, double ypos);
    static void glfw_scroll(GLFWwindow* window, double xoffset, double yoffset);
    static void glfw_resize(GLFWwindow* window, int width, int height);
    static void glfw_drop(GLFWwindow* window, int count, const char** paths);
    static void glfw_scale(GLFWwindow* window, float xscale, float yscale);

#if __EMSCRIPTEN__
    static EM_BOOL emscripten_touchstart(int, const EmscriptenTouchEvent*, void*);
    static EM_BOOL emscripten_touchmove(int, const EmscriptenTouchEvent*, void*);
    static EM_BOOL emscripten_touchend(int, const EmscriptenTouchEvent*, void*);
#endif

    // the active window
    static Window* instance_;

    // GLFW window pointer
    GLFWwindow* window_;

    // window title
    std::string title_;

    // current viewport dimension
    int width_, height_;

    // highDPI scaling
    float scaling_{1.0};

    // whether to show ImGUI menu
    bool show_imgui_;
    // scale ImGUI menu
    float imgui_scale_{1.0};
    // show ImGUI help dialog
    bool show_help_{false};
    // items for ImGUI help dialog
    std::vector<std::pair<std::string, std::string>> help_items_;

    // which mouse buttons and modifier keys are pressed down
    std::array<bool, 7> button_;
    bool ctrl_pressed_;
    bool alt_pressed_;
    bool shift_pressed_;

    // fullscreen-related backups
    int backup_xpos_;
    int backup_ypos_;
    int backup_width_;
    int backup_height_;
    bool is_fullscreen() const;
    void enter_fullscreen();
    void exit_fullscreen();

    // screenshot number
    unsigned int screenshot_number_{0};
};

} // namespace pmp
