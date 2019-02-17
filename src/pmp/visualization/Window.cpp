//=============================================================================
// Copyright (C) 2011-2019 The pmp-library developers
//
// This file is part of the Polygon Mesh Processing Library.
// Distributed under the terms of the MIT license, see LICENSE.txt for details.
//
// SPDX-License-Identifier: MIT
//=============================================================================

#include "Window.h"
#include <algorithm>

#include <imgui_glfw.h>
#include <lato-font.h>

#if __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#endif

//=============================================================================

namespace pmp {

//=============================================================================

Window* Window::instance_ = nullptr;

//-----------------------------------------------------------------------------

Window::Window(const char* title, int width, int height, bool showgui)
    : width_(width), height_(height), show_imgui_(showgui)
{
    // initialize glfw window
    if (!glfwInit())
        exit(EXIT_FAILURE);

    // request core profile and OpenGL version 3.2
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_SAMPLES, 4);

    window_ = glfwCreateWindow(width, height, title, nullptr, nullptr);

    if (!window_)
    {
        glfwTerminate();
        std::cerr << "Cannot create GLFW window.\n";
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window_);
    instance_ = this;

    // enable v-sync
    glfwSwapInterval(1);

    // now that we have a GL context, initialize GLEW
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        std::cerr << "Error initializing GLEW: " << glewGetErrorString(err)
                  << std::endl;
        exit(1);
    }

    // debug: print GL and GLSL version
    std::cout << "GLEW   " << glewGetString(GLEW_VERSION) << std::endl;
    std::cout << "GL     " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL   " << glGetString(GL_SHADING_LANGUAGE_VERSION)
              << std::endl;

    // call glGetError once to clear error queue
    glGetError();

    // detect highDPI scaling
    int window_width, window_height, framebuffer_width, framebuffer_height;
    glfwGetWindowSize(window_, &window_width, &window_height);
    glfwGetFramebufferSize(window_, &framebuffer_width, &framebuffer_height);
    scaling_ = framebuffer_width / window_width;
    width_ = framebuffer_width;
    height_ = framebuffer_height;
    if (scaling_ != 1)
        std::cout << "highDPI scaling: " << scaling_ << std::endl;

    // register glfw callbacks
    glfwSetErrorCallback(glfw_error);
    glfwSetCharCallback(window_, glfw_character);
    glfwSetKeyCallback(window_, glfw_keyboard);
    glfwSetCursorPosCallback(window_, glfw_motion);
    glfwSetMouseButtonCallback(window_, glfw_mouse);
    glfwSetScrollCallback(window_, glfw_scroll);
    glfwSetFramebufferSizeCallback(window_, glfw_resize);

    // setup imgui
    init_imgui();
}

//-----------------------------------------------------------------------------

void Window::init_imgui()
{
    ImGui_Init(window_, false);

    // load Lato font from pre-compiled ttf file
    ImFontConfig config;
    config.OversampleH = 2;
    config.OversampleV = 2;
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromMemoryCompressedTTF(LatoLatin_compressed_data,
                                             LatoLatin_compressed_size, 14);

    // window style
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 4.0f;
    style.FrameRounding = 4.0f;
    style.GrabMinSize = 10.0f;
    style.GrabRounding = 4.0f;

    // color scheme adapted from
    // https://github.com/ocornut/imgui/pull/511#issuecomment-175719267
    style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.90f, 0.90f, 0.90f, 0.70f);
    style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.90f, 0.90f, 0.90f, 0.90f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.16f, 0.62f, 0.87f, 0.40f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.16f, 0.62f, 0.87f, 0.67f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.16f, 0.62f, 0.87f, 0.80f);
    style.Colors[ImGuiCol_TitleBgCollapsed] =
        ImVec4(0.16f, 0.62f, 0.87f, 0.40f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.16f, 0.62f, 0.87f, 0.80f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.69f, 0.69f, 0.69f, 0.80f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] =
        ImVec4(0.49f, 0.49f, 0.49f, 0.80f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] =
        ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
    style.Colors[ImGuiCol_ComboBg] = ImVec4(0.86f, 0.86f, 0.86f, 0.99f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.16f, 0.62f, 0.87f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.16f, 0.62f, 0.87f, 0.78f);
    style.Colors[ImGuiCol_SliderGrabActive] =
        ImVec4(0.16f, 0.62f, 0.87f, 1.00f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.16f, 0.62f, 0.87f, 0.40f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.16f, 0.62f, 0.87f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.16f, 0.62f, 0.87f, 1.00f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.16f, 0.62f, 0.87f, 0.31f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.16f, 0.62f, 0.87f, 0.80f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.16f, 0.62f, 0.87f, 1.00f);
    style.Colors[ImGuiCol_Column] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.16f, 0.62f, 0.87f, 0.78f);
    style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.16f, 0.62f, 0.87f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
    style.Colors[ImGuiCol_ResizeGripHovered] =
        ImVec4(0.16f, 0.62f, 0.87f, 0.67f);
    style.Colors[ImGuiCol_ResizeGripActive] =
        ImVec4(0.16f, 0.62f, 0.87f, 0.95f);
    style.Colors[ImGuiCol_CloseButton] = ImVec4(0.59f, 0.59f, 0.59f, 0.50f);
    style.Colors[ImGuiCol_CloseButtonHovered] =
        ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
    style.Colors[ImGuiCol_CloseButtonActive] =
        ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered] =
        ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered] =
        ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.16f, 0.62f, 0.87f, 0.35f);
    style.Colors[ImGuiCol_ModalWindowDarkening] =
        ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}

//-----------------------------------------------------------------------------

Window::~Window()
{
    // terminate imgui
    ImGui_Shutdown();

    // terminate GLFW
    glfwTerminate();
}

//-----------------------------------------------------------------------------

int Window::run()
{
#if __EMSCRIPTEN__
    emscripten_set_main_loop(Window::render_frame, 0, 1);
#else
    while (!glfwWindowShouldClose(window_))
    {
        Window::render_frame();
    }
#endif
    glfwDestroyWindow(window_);
    return EXIT_SUCCESS;
}

//-----------------------------------------------------------------------------

void Window::render_frame()
{
#if __EMSCRIPTEN__
    // determine correct canvas/framebuffer size
    int w, h, f;
    double dw, dh;
    emscripten_get_canvas_element_size("#canvas", &w, &h);
    emscripten_get_element_css_size("#canvas", &dw, &dh);
    if (w != int(dw) || h != int(dh))
    {
        // set canvas size to match element css size
        w = int(dw);
        h = int(dh);
        // set canvas size
        emscripten_set_canvas_element_size("#canvas", w, h);
        // inform GLFW of this change, since ImGUI asks GLFW for window size
        glfwSetWindowSize(instance_->window_, w, h);
    }
#endif

    // do some computations
    instance_->do_processing();

    // preapre and process ImGUI elements
    if (instance_->show_imgui())
    {
        ImGui_NewFrame();
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Once);
        ImGui::Begin(
            "Mesh Info", nullptr,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);
        instance_->process_imgui();
        ImGui::End();
    }

    // draw scene
    instance_->display();

    // draw GUI
    if (instance_->show_imgui())
    {
        ImGui::Render();
    }

#if __EMSCRIPTEN__
    // to avoid problems with premultiplied alpha in WebGL,
    // clear alpha values to 1.0
    // see here: https://webgl2fundamentals.org/webgl/lessons/webgl-and-alpha.html
    // (no way to disable premultiplied-alpha in emscripten-glfw)
    GLfloat rgba[4];
    glGetFloatv(GL_COLOR_CLEAR_VALUE, rgba);
    glClearColor(1, 1, 1, 1);
    glColorMask(false, false, false, true);
    glClear(GL_COLOR_BUFFER_BIT);
    glColorMask(true, true, true, true);
    glClearColor(rgba[0], rgba[1], rgba[2], rgba[3]);
#endif

    // swap buffers
    glfwSwapBuffers(instance_->window_);

    // handle events
    glfwPollEvents();
}

//-----------------------------------------------------------------------------

void Window::glfw_error(int error, const char* description)
{
    std::cerr << "error (" << error << "):" << description << std::endl;
}

//-----------------------------------------------------------------------------

void Window::glfw_character(GLFWwindow* window, unsigned int c)
{
    ImGui_CharCallback(window, c);
    if (!ImGui::GetIO().WantCaptureKeyboard)
    {
        instance_->character(c);
    }
}

//-----------------------------------------------------------------------------

void Window::glfw_keyboard(GLFWwindow* window, int key, int scancode,
                           int action, int mods)
{
    ImGui_KeyCallback(window, key, scancode, action, mods);
    if (!ImGui::GetIO().WantCaptureKeyboard)
    {
        instance_->keyboard(key, scancode, action, mods);
    }
}

//-----------------------------------------------------------------------------

void Window::glfw_motion(GLFWwindow* /*window*/, double xpos, double ypos)
{
    // correct for highDPI scaling
    instance_->motion(instance_->scaling_ * xpos, instance_->scaling_ * ypos);
}

//-----------------------------------------------------------------------------

void Window::glfw_mouse(GLFWwindow* window, int button, int action, int mods)
{
    ImGui_MouseButtonCallback(window, button, action, mods);
    if (!ImGui::GetIO().WantCaptureMouse)
    {
        instance_->mouse(button, action, mods);
    }
}

//-----------------------------------------------------------------------------

void Window::glfw_scroll(GLFWwindow* window, double xoffset, double yoffset)
{
#ifdef __EMSCRIPTEN__
    yoffset *= -0.02;
#endif

    ImGui_ScrollCallback(window, xoffset, yoffset);
    if (!ImGui::GetIO().WantCaptureMouse)
    {
        instance_->scroll(xoffset, yoffset);
    }
}

//-----------------------------------------------------------------------------

void Window::glfw_resize(GLFWwindow* /*window*/, int width, int height)
{
    instance_->width_ = width;
    instance_->height_ = height;
    instance_->resize(width, height);
}

//-----------------------------------------------------------------------------

void Window::cursor_pos(double& x, double& y) const
{
    glfwGetCursorPos(window_, &x, &y);
    x *= instance_->scaling_;
    y *= instance_->scaling_;
}

//=============================================================================
} // namespace pmp
//=============================================================================
