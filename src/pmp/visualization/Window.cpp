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

#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
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
    : width_(width), height_(height), 
      scaling_(1), pixel_ratio_(1),
      show_imgui_(showgui), imgui_scale_(1.0),
      show_help_(false)
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
    width_ = framebuffer_width;
    height_ = framebuffer_height;
#ifndef __EMSCRIPTEN__
    scaling_ = framebuffer_width / window_width;
    if (scaling_ != 1)
        std::cout << "highDPI scaling: " << scaling_ << std::endl;
#else
    pixel_ratio_ = emscripten_get_device_pixel_ratio();
    if (pixel_ratio_ != 1)
        std::cout << "highDPI scaling: " << scaling_ << std::endl;
    imgui_scale_ = pixel_ratio_;
#endif


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

    // add help items
    add_help_item("G", "Toggle GUI dialog");
    add_help_item("PageUp/Down", "Scale GUI dialogs");
    add_help_item("Esc/Q", "Quit application");
}

//-----------------------------------------------------------------------------

Window::~Window()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window_);
    glfwTerminate();
}

//-----------------------------------------------------------------------------

void Window::init_imgui()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = nullptr;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window_, false);
#ifdef __EMSCRIPTEN__
    const char* glsl_version = "#version 300 es";
#else
    const char* glsl_version = "#version 330";
#endif
    ImGui_ImplOpenGL3_Init(glsl_version);

    // load Lato font from pre-compiled ttf file
    io.Fonts->AddFontFromMemoryCompressedTTF(LatoLatin_compressed_data,
                                             LatoLatin_compressed_size, 
                                             14*imgui_scale_);

    // window style
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowBorderSize = 0;
    style.WindowRounding   = 4 * imgui_scale_;
    style.FrameRounding    = 4 * imgui_scale_;
    style.GrabMinSize      = 10 * imgui_scale_;
    style.GrabRounding     = 4 * imgui_scale_;

    // color scheme adapted from
    // https://github.com/ocornut/imgui/pull/511#issuecomment-175719267
    style.Colors[ImGuiCol_Text]                 = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled]         = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    style.Colors[ImGuiCol_WindowBg]             = ImVec4(0.90f, 0.90f, 0.90f, 0.70f);
    style.Colors[ImGuiCol_ChildBg]              = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_PopupBg]              = ImVec4(0.90f, 0.90f, 0.90f, 0.90f);
    style.Colors[ImGuiCol_Border]               = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
    style.Colors[ImGuiCol_BorderShadow]         = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
    style.Colors[ImGuiCol_FrameBg]              = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered]       = ImVec4(0.16f, 0.62f, 0.87f, 0.40f);
    style.Colors[ImGuiCol_FrameBgActive]        = ImVec4(0.16f, 0.62f, 0.87f, 0.67f);
    style.Colors[ImGuiCol_TitleBg]              = ImVec4(0.16f, 0.62f, 0.87f, 0.80f);
    style.Colors[ImGuiCol_TitleBgActive]        = ImVec4(0.16f, 0.62f, 0.87f, 0.80f);
    style.Colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(0.16f, 0.62f, 0.87f, 0.40f);
    style.Colors[ImGuiCol_MenuBarBg]            = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarBg]          = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
    style.Colors[ImGuiCol_ScrollbarGrab]        = ImVec4(0.69f, 0.69f, 0.69f, 0.80f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.49f, 0.49f, 0.49f, 0.80f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
    style.Colors[ImGuiCol_CheckMark]            = ImVec4(0.16f, 0.62f, 0.87f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab]           = ImVec4(0.16f, 0.62f, 0.87f, 0.78f);
    style.Colors[ImGuiCol_SliderGrabActive]     = ImVec4(0.16f, 0.62f, 0.87f, 1.00f);
    style.Colors[ImGuiCol_Button]               = ImVec4(0.16f, 0.62f, 0.87f, 0.40f);
    style.Colors[ImGuiCol_ButtonHovered]        = ImVec4(0.16f, 0.62f, 0.87f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive]         = ImVec4(0.16f, 0.62f, 0.87f, 1.00f);
    style.Colors[ImGuiCol_Header]               = ImVec4(0.16f, 0.62f, 0.87f, 0.31f);
    style.Colors[ImGuiCol_HeaderHovered]        = ImVec4(0.16f, 0.62f, 0.87f, 0.80f);
    style.Colors[ImGuiCol_HeaderActive]         = ImVec4(0.16f, 0.62f, 0.87f, 1.00f);
    //style.Colors[ImGuiCol_Column]               = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    //style.Colors[ImGuiCol_ColumnHovered]        = ImVec4(0.16f, 0.62f, 0.87f, 0.78f);
    //style.Colors[ImGuiCol_ColumnActive]         = ImVec4(0.16f, 0.62f, 0.87f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip]           = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
    style.Colors[ImGuiCol_ResizeGripHovered]    = ImVec4(0.16f, 0.62f, 0.87f, 0.67f);
    style.Colors[ImGuiCol_ResizeGripActive]     = ImVec4(0.16f, 0.62f, 0.87f, 0.95f);
    //style.Colors[ImGuiCol_CloseButton]          = ImVec4(0.59f, 0.59f, 0.59f, 0.50f);
    //style.Colors[ImGuiCol_CloseButtonHovered]   = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
    //style.Colors[ImGuiCol_CloseButtonActive]    = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
    style.Colors[ImGuiCol_PlotLines]            = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered]     = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram]        = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg]       = ImVec4(0.16f, 0.62f, 0.87f, 0.35f);
    style.Colors[ImGuiCol_ModalWindowDimBg]     = ImVec4(0.20f, 0.20f, 0.20f, 0.70f);
}

//-----------------------------------------------------------------------------

void Window::scale_imgui(float scale)
{
    // scale imgui scale by new factor
    imgui_scale_ *= scale;

    // reload font
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear();
    io.Fonts->AddFontFromMemoryCompressedTTF(
            LatoLatin_compressed_data,
            LatoLatin_compressed_size, 
            14*imgui_scale_);

    // trigger font texture regeneration
    ImGui_ImplOpenGL3_DestroyFontsTexture();
    ImGui_ImplOpenGL3_CreateFontsTexture();

    // adjust element styles (scaled version of default style or pmp style)
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowPadding          = ImVec2(8*scale, 8*scale);
    style.WindowRounding         = 4 * scale;
    style.FramePadding           = ImVec2(4*scale, 2*scale);
    style.FrameRounding          = 4 * scale;
    style.ItemSpacing            = ImVec2(8*scale, 4*scale);
    style.ItemInnerSpacing       = ImVec2(4*scale, 4*scale);
    style.IndentSpacing          = 21 * scale;
    style.ColumnsMinSpacing      =  6 * scale;
    style.ScrollbarSize          = 16 * scale;
    style.ScrollbarRounding      =  9 * scale;
    style.GrabMinSize            = 10 * scale;
    style.GrabRounding           =  4 * scale;
    style.TabRounding            =  4 * scale;
    style.DisplayWindowPadding   = ImVec2(19*scale, 19*scale);
    style.DisplaySafeAreaPadding = ImVec2(3*scale, 3*scale);
}

//-----------------------------------------------------------------------------

void Window::add_help_item(std::string key, std::string description, int pos)
{
    if (pos==-1)
    {
        help_items_.push_back( std::make_pair(key, description) );
    }
    else
    {
        auto it = help_items_.begin();
        it += pos;
        help_items_.insert( it, std::make_pair(key, description) );
    }
}

//-----------------------------------------------------------------------------

void Window::show_help()
{
    if (!show_help_) return;

    ImGui::OpenPopup("Key Bindings");

    ImGui::SetNextWindowFocus();
    if (ImGui::BeginPopupModal("Key Bindings", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Columns(2, "help items");
        ImGui::SetColumnWidth(0, 100*imgui_scale_);
        ImGui::SetColumnWidth(1, 200*imgui_scale_);
        ImGui::Separator();
        ImGui::Text("Trigger"); ImGui::NextColumn();
        ImGui::Text("Description"); ImGui::NextColumn();
        ImGui::Separator();

        for (const auto& item: help_items_)
        {
            ImGui::Text("%s", item.first.c_str()); ImGui::NextColumn();
            ImGui::Text("%s", item.second.c_str()); ImGui::NextColumn();
        }

        ImGui::Columns(1);
        ImGui::Separator();

        if (ImGui::Button("OK", ImVec2(300*imgui_scale_,0)))
        {
            show_help_ = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
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
    glfwMakeContextCurrent(instance_->window_);

#if __EMSCRIPTEN__
    // determine correct canvas/framebuffer size
    int w, h, f;
    double dw, dh;
    emscripten_get_canvas_element_size("#canvas", &w, &h);
    emscripten_get_element_css_size("#canvas", &dw, &dh);
    double s = instance_->pixel_ratio_;
    if (w != int(dw*s) || h != int(dh*s))
    {
        // set canvas size to match element css size
        w = int(dw*s);
        h = int(dh*s);
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
        // start imgui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // prepare, process, and finish applications ImGUI dialog
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Once);
        ImGui::Begin(
                "Mesh Info", nullptr,
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("Press '?' for help");
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        instance_->process_imgui();
        ImGui::End();
        
        // show imgui help
        instance_->show_help();

        ImGui::Render();
    }

    // draw scene
    instance_->display();

    // draw GUI
    if (instance_->show_imgui())
    {
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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
    ImGui_ImplGlfw_CharCallback(window, c);
    if (!ImGui::GetIO().WantCaptureKeyboard)
    {
        instance_->character(c);
    }
}

//-----------------------------------------------------------------------------

void Window::glfw_keyboard(GLFWwindow* window, int key, int scancode,
                           int action, int mods)
{
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
    if (!ImGui::GetIO().WantCaptureKeyboard)
    {
        instance_->keyboard(key, scancode, action, mods);
    }
}

//-----------------------------------------------------------------------------

void Window::character(unsigned int c)
{
    switch (c)
    {
        case 63: // question mark
            show_help_ = true;
            break;
    }
}

//-----------------------------------------------------------------------------

void Window::keyboard(int key, int /*code*/, int action, int /*mods*/)
{
    if (action != GLFW_PRESS && action != GLFW_REPEAT)
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
        case GLFW_KEY_G:
        {
            show_imgui(!show_imgui());
            break;
        }

        case GLFW_KEY_PAGE_UP:
        {
            scale_imgui(1.25);
            break;
        }

        case GLFW_KEY_PAGE_DOWN:
        {
            scale_imgui(0.8);
            break;
        }
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
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
    if (!ImGui::GetIO().WantCaptureMouse)
    {
        instance_->mouse(button, action, mods);
    }
}

//-----------------------------------------------------------------------------

void Window::glfw_scroll(GLFWwindow* window, double xoffset, double yoffset)
{
#ifdef __EMSCRIPTEN__
    yoffset = -yoffset;

    // thresholding for cross-browser handling
    const float t = 5;
    if (yoffset > t) yoffset=t;
    else if (yoffset < -t) yoffset = -t;
#endif

    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
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
