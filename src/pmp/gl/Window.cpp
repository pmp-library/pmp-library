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

#include "Window.h"
#include <algorithm>

#if __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

//=============================================================================

namespace pmp {

//=============================================================================

Window* Window::m_instance = nullptr;

//-----------------------------------------------------------------------------

Window::Window(const char* title, int width, int height)
    : m_width(width), m_height(height)
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
    if(err != GLEW_OK)
    {
        std::cerr << "Error initializing GLEW: " << glewGetErrorString(err) << std::endl;
        exit(1);
    }


    // debug: print GL and GLSL version
    std::cout << "GLEW   " << glewGetString(GLEW_VERSION) << std::endl;
    std::cout << "GL     " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL   " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;


    // call glGetError once to clear error queue
    glGetError();

    // detect highDPI scaling
    int windowWidth, windowHeight, framebufferWidth, framebufferHeight;
    glfwGetWindowSize(m_window, &windowWidth, &windowHeight);
    glfwGetFramebufferSize(m_window, &framebufferWidth, &framebufferHeight);
    m_scaling = framebufferWidth / windowWidth;
    m_width   = framebufferWidth;
    m_height  = framebufferHeight;
    if (m_scaling != 1) std::cout << "highDPI scaling: " << m_scaling << std::endl;

    // register glfw callbacks
    glfwSetErrorCallback(glfwError);
    glfwSetKeyCallback(m_window, glfwKeyboard);
    glfwSetCursorPosCallback(m_window, glfwMotion);
    glfwSetMouseButtonCallback(m_window, glfwMouse);
    glfwSetScrollCallback(m_window, glfwScroll);
    glfwSetFramebufferSizeCallback(m_window, glfwResize);
}

//-----------------------------------------------------------------------------

Window::~Window()
{
    // terminate GLFW
    glfwTerminate();
}

//-----------------------------------------------------------------------------

int Window::run()
{
#if __EMSCRIPTEN__
    emscripten_set_main_loop(Window::emscripten_render_loop, 0, 1);
#else
    while (!glfwWindowShouldClose(m_window))
    {
        // draw scene
        display();

        // swap buffers
        glfwSwapBuffers(m_window);

        // handle events
        //glfwPollEvents();
        glfwWaitEvents();
    }
#endif

    glfwDestroyWindow(m_window);

    return EXIT_SUCCESS;
}


//-----------------------------------------------------------------------------


void Window::emscripten_render_loop()
{
      // draw scene
      m_instance->display();

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

void Window::glfwKeyboard(GLFWwindow* window, int key, int scancode,
                              int action, int mods)
{
    m_instance->keyboard(window, key, scancode, action, mods);
}

//-----------------------------------------------------------------------------

void Window::glfwMotion(GLFWwindow* window, double xpos, double ypos)
{
    // correct for highDPI scaling
    m_instance->motion(window, m_instance->m_scaling*xpos, m_instance->m_scaling*ypos);
}

//-----------------------------------------------------------------------------

void Window::glfwMouse(GLFWwindow* window, int button, int action, int mods)
{
    m_instance->mouse(window, button, action, mods);
}

//-----------------------------------------------------------------------------

void Window::glfwScroll(GLFWwindow* window, double xoffset, double yoffset)
{
    m_instance->scroll(window, xoffset, yoffset);
}

//-----------------------------------------------------------------------------

void Window::glfwResize(GLFWwindow* window, int width, int height)
{
    m_instance->m_width  = width;
    m_instance->m_height = height;
    m_instance->resize(window, width, height);
}

//=============================================================================
} // namespace pmp
//=============================================================================
