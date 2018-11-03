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

Window* Window::m_instance = nullptr;

//-----------------------------------------------------------------------------

Window::Window(const char* title, int width, int height, bool showgui)
    : m_width(width), m_height(height), m_showImGUI(showgui)
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

    m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);

    if (!m_window)
    {
        glfwTerminate();
        std::cerr << "Cannot create GLFW window.\n";
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(m_window);
    m_instance = this;

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
    int windowWidth, windowHeight, framebufferWidth, framebufferHeight;
    glfwGetWindowSize(m_window, &windowWidth, &windowHeight);
    glfwGetFramebufferSize(m_window, &framebufferWidth, &framebufferHeight);
    m_scaling = framebufferWidth / windowWidth;
    m_width = framebufferWidth;
    m_height = framebufferHeight;
    if (m_scaling != 1)
        std::cout << "highDPI scaling: " << m_scaling << std::endl;

    // register glfw callbacks
    glfwSetErrorCallback(glfwError);
    glfwSetCharCallback(m_window, glfwCharacter);
    glfwSetKeyCallback(m_window, glfwKeyboard);
    glfwSetCursorPosCallback(m_window, glfwMotion);
    glfwSetMouseButtonCallback(m_window, glfwMouse);
    glfwSetScrollCallback(m_window, glfwScroll);
    glfwSetFramebufferSizeCallback(m_window, glfwResize);

    // setup imgui
    initImGUI();
}

//-----------------------------------------------------------------------------

void Window::initImGUI()
{
    ImGui_Init(m_window, false);

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
    while (!glfwWindowShouldClose(m_window))
    {
        Window::render_frame();
    }
#endif
    glfwDestroyWindow(m_window);
    return EXIT_SUCCESS;
}

//-----------------------------------------------------------------------------

void Window::render_frame()
{
#if __EMSCRIPTEN__
    // determine correct canvas/framebuffer size
    int w, h, f;
    double dw, dh;
    emscripten_get_canvas_size(&w, &h, &f);
    emscripten_get_element_css_size(nullptr, &dw, &dh);
    if (w != int(dw) || h != int(dh))
    {
        w = int(dw);
        h = int(dh);
        emscripten_set_canvas_size(w, h);
        glfwResize(m_instance->m_window, w, h);
        ;
    }
#endif

    // do some computations
    m_instance->doProcessing();

    // preapre and process ImGUI elements
    if (m_instance->showImGUI())
    {
        ImGui_NewFrame();
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Once);
        ImGui::Begin(
            "Mesh Info", nullptr,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);
        m_instance->processImGUI();
        ImGui::End();
    }

    // draw scene
    m_instance->display();

    // draw GUI
    if (m_instance->showImGUI())
    {
        ImGui::Render();
    }

    // swap buffers
    glfwSwapBuffers(m_instance->m_window);

    // handle events
    glfwPollEvents();
}

//-----------------------------------------------------------------------------

void Window::glfwError(int error, const char* description)
{
    std::cerr << "error (" << error << "):" << description << std::endl;
}

//-----------------------------------------------------------------------------

void Window::glfwCharacter(GLFWwindow* window, unsigned int c)
{
    ImGui_CharCallback(window, c);
    if (!ImGui::GetIO().WantCaptureKeyboard)
    {
        m_instance->character(c);
    }
}

//-----------------------------------------------------------------------------

void Window::glfwKeyboard(GLFWwindow* window, int key, int scancode, int action,
                          int mods)
{
    ImGui_KeyCallback(window, key, scancode, action, mods);
    if (!ImGui::GetIO().WantCaptureKeyboard)
    {
        m_instance->keyboard(key, scancode, action, mods);
    }
}

//-----------------------------------------------------------------------------

void Window::glfwMotion(GLFWwindow* /*window*/, double xpos, double ypos)
{
    // correct for highDPI scaling
    m_instance->motion(m_instance->m_scaling * xpos,
                       m_instance->m_scaling * ypos);
}

//-----------------------------------------------------------------------------

void Window::glfwMouse(GLFWwindow* window, int button, int action, int mods)
{
    ImGui_MouseButtonCallback(window, button, action, mods);
    if (!ImGui::GetIO().WantCaptureMouse)
    {
        m_instance->mouse(button, action, mods);
    }
}

//-----------------------------------------------------------------------------

void Window::glfwScroll(GLFWwindow* window, double xoffset, double yoffset)
{
#ifdef __EMSCRIPTEN__
    yoffset *= -0.02;
#endif

    ImGui_ScrollCallback(window, xoffset, yoffset);
    if (!ImGui::GetIO().WantCaptureMouse)
    {
        m_instance->scroll(xoffset, yoffset);
    }
}

//-----------------------------------------------------------------------------

void Window::glfwResize(GLFWwindow* /*window*/, int width, int height)
{
    m_instance->m_width = width;
    m_instance->m_height = height;
    m_instance->resize(width, height);
}

//-----------------------------------------------------------------------------

void Window::cursorPos(double& x, double& y) const
{
    glfwGetCursorPos(m_window, &x, &y);
    x *= m_instance->m_scaling;
    y *= m_instance->m_scaling;
}

//=============================================================================
} // namespace pmp
//=============================================================================
