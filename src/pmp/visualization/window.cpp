// Copyright 2011-2025 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"

#include "window.h"
#include "font-lato.h"
#include "font-awesome.h"
#include "font-icons.h"

#include <sstream>

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <stb_image_write.h>

#if __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <GL/gl.h>
#else
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#endif

static inline float  ImTrunc(float f)         { return (float)(int)(f); }
static inline ImVec2 ImTrunc(const ImVec2& v) { return ImVec2((float)(int)(v.x), (float)(int)(v.y)); }

namespace pmp {

Window* Window::instance_ = nullptr;

Window::Window(const char* title, int width, int height, bool showgui)
    : title_(title), width_(width), height_(height), show_imgui_(showgui)
{
    // initialize glfw window
    if (!glfwInit())
        exit(EXIT_FAILURE);

    // remove spaces from title
    for (auto& c : title_)
        if (c == ' ')
            c = '_';

    // request core profile and OpenGL version 3.2
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

    window_ = glfwCreateWindow(width, height, title, nullptr, nullptr);

    if (!window_)
    {
        std::cerr << "Cannot create GLFW window.\n";
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window_);
    instance_ = this;

#ifndef __EMSCRIPTEN__
    const auto version = gladLoadGL(glfwGetProcAddress);
    if (version == 0)
    {
        std::cout << "Failed to initialize OpenGL context\n";
        exit(EXIT_FAILURE);
    }
#endif

    // check for sufficient OpenGL version
    GLint major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    const GLint glversion = 10 * major + minor;
#ifdef __EMSCRIPTEN__
    if (glversion < 30)
    {
        // clang-format will line-break strings, which will break EM_ASM
        // clang-format off
        std::cerr << "Cannot get WebGL2 context. Try using Firefox or Chrome/Chromium.\n";
        EM_ASM(alert("Cannot get WebGL2 context. Try using Firefox or Chrome/Chromium."));
        // clang-format on
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
#else
    if (glversion < 32)
    {
        std::cerr << "Cannot get modern OpenGL context.\n";
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
#endif

    // enable v-sync
    glfwSwapInterval(1);

    // debug: print GL and GLSL version
    std::cout << "GL     " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL   " << glGetString(GL_SHADING_LANGUAGE_VERSION)
              << std::endl;

    // call glGetError once to clear error queue
    glGetError();

    // detect highDPI framebuffer scaling and UI scaling
    int window_width, window_height, framebuffer_width, framebuffer_height;
    glfwGetWindowSize(window_, &window_width, &window_height);
    glfwGetFramebufferSize(window_, &framebuffer_width, &framebuffer_height);
    width_ = framebuffer_width;
    height_ = framebuffer_height;
    scaling_ = static_cast<float>(framebuffer_width) /
               static_cast<float>(window_width);
    if (scaling_ != 1)
        std::cout << "highDPI scaling: " << scaling_ << std::endl;

#if !defined(__APPLE__) && !defined(__EMSCRIPTEN__)
    float sx, sy;
    glfwGetWindowContentScale(window_, &sx, &sy);
    imgui_scale_ = std::max(1.0f, 0.5f * (sx + sy));
    if (imgui_scale_ != 1.0f)
        std::cout << "UI scaling: " << imgui_scale_ << std::endl;
#endif

    // register glfw callbacks
    glfwSetErrorCallback(glfw_error);
    glfwSetCharCallback(window_, glfw_character);
    glfwSetKeyCallback(window_, glfw_keyboard);
    glfwSetCursorPosCallback(window_, glfw_motion);
    glfwSetMouseButtonCallback(window_, glfw_mouse);
    glfwSetScrollCallback(window_, glfw_scroll);
    glfwSetFramebufferSizeCallback(window_, glfw_resize);
    glfwSetDropCallback(window_, glfw_drop);
    glfwSetWindowContentScaleCallback(window_, glfw_scale);

#if defined(__EMSCRIPTEN__)
    // touch event handlers
    emscripten_set_touchstart_callback("#canvas", nullptr, true,
                                       emscripten_touchstart);
    emscripten_set_touchmove_callback("#canvas", nullptr, true,
                                      emscripten_touchmove);
    emscripten_set_touchend_callback("#canvas", nullptr, true,
                                     emscripten_touchend);
#endif

    // setup imgui
    init_imgui();

    // add help items
    add_help_item("F", "Toggle fullscreen mode");
    add_help_item("G", "Toggle GUI dialog");
    add_help_item("PageUp/Down", "Scale GUI dialogs");
#ifndef __EMSCRIPTEN__
    add_help_item("PrtScr/P", "Save screenshot");
    add_help_item("Esc/Q", "Quit application");
#endif

    // init mouse button state and modifiers
    for (bool& i : button_)
        i = false;
    ctrl_pressed_ = shift_pressed_ = alt_pressed_ = false;
}

Window::~Window()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate(); // this automatically destroys remaining windows
}

void Window::init_imgui()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.IniFilename = nullptr;
    ImGui_ImplGlfw_InitForOpenGL(window_, false);
#ifdef __EMSCRIPTEN__
    ImGui_ImplOpenGL3_Init("#version 300 es");
    ImGui_ImplGlfw_InstallEmscriptenCallbacks(window_, "#canvas");
#else
    ImGui_ImplOpenGL3_Init("#version 330");
#endif

    // setup font and border radii
    scale_imgui(1.0);

    // setup colors
    color_mode(color_mode_);
}

void Window::color_mode(ColorMode c)
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    switch (color_mode_ = c)
    {
        case LightMode:
            ImGui::StyleColorsLight();
            colors[ImGuiCol_WindowBg] = ImVec4(0.90f, 0.90f, 0.90f, 0.90f);
            colors[ImGuiCol_Border] = ImVec4(0, 0, 0, 0);
            clear_color_ = vec3(1.0, 1.0, 1.0);
            break;

        case DarkMode:
            ImGui::StyleColorsDark();
            colors[ImGuiCol_WindowBg] = ImVec4(0.1, 0.1, 0.1, 0.9);
            colors[ImGuiCol_Border] = ImVec4(0, 0, 0, 0);
            clear_color_ = vec3(0.11, 0.12, 0.13);
            break;
    }
}

void Window::scale_imgui(float scale)
{
    // scale imgui scale by new factor
    imgui_scale_ *= scale;

    // load Lato font 
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear();
    io.Fonts->AddFontFromMemoryCompressedTTF(LatoLatin_compressed_data,
                                             LatoLatin_compressed_size,
                                             16 * imgui_scaling());
    
    // load & merge FontAwesome
    static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
    ImFontConfig config;
    config.MergeMode = true;
    io.Fonts->AddFontFromMemoryCompressedTTF(
        FontAwesome_compressed_data, FontAwesome_compressed_size,
        16 * imgui_scaling(), &config, icons_ranges);

    // trigger font texture regeneration
    ImGui_ImplOpenGL3_DestroyFontsTexture();
    ImGui_ImplOpenGL3_CreateFontsTexture();

    // adjust element styles (scaled version of default style or pmp style)
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowPadding = ImTrunc(ImVec2(8,8) * imgui_scale_);
    style.WindowRounding = ImTrunc(4 * imgui_scale_);
    style.WindowMinSize = ImTrunc(ImVec2(32,32) * imgui_scale_);
    style.ChildRounding = ImTrunc(4 * imgui_scale_);
    style.PopupRounding = ImTrunc(4 * imgui_scale_);
    style.FramePadding = ImTrunc(ImVec2(4,3) * imgui_scale_);
    style.FrameRounding = ImTrunc(4 * imgui_scale_);
    style.ItemSpacing = ImTrunc(ImVec2(4,4) * imgui_scale_);
    style.ItemInnerSpacing = ImTrunc(ImVec2(4,4) * imgui_scale_);
    style.CellPadding = ImTrunc(ImVec2(4,2) * imgui_scale_);
    style.TouchExtraPadding = ImVec2(0,0);
    style.IndentSpacing = ImTrunc(21 * imgui_scale_);
    style.ColumnsMinSpacing = ImTrunc(6 * imgui_scale_);
    style.ScrollbarSize = ImTrunc(14 * imgui_scale_);
    style.ScrollbarRounding = ImTrunc(9 * imgui_scale_);
    style.GrabMinSize = ImTrunc(10 * imgui_scale_);
    style.GrabRounding = ImTrunc(4 * imgui_scale_);
    style.LogSliderDeadzone = ImTrunc(4 * imgui_scale_);
    style.TabRounding = ImTrunc(4 * imgui_scale_);
    style.TabMinWidthForCloseButton = ImTrunc(0 * imgui_scale_);
    style.TabBarOverlineSize = ImTrunc(1 * imgui_scale_);
    style.SeparatorTextPadding = ImTrunc(ImVec2(20,3) * imgui_scale_);
    style.DisplayWindowPadding = ImTrunc(ImVec2(19,19) * imgui_scale_);
    style.DisplaySafeAreaPadding = ImTrunc(ImVec2(3,3) * imgui_scale_);
}

void Window::add_help_item(std::string key, std::string description, int pos)
{
    if (pos == -1)
    {
        help_items_.emplace_back(key, description);
    }
    else
    {
        auto it = help_items_.begin();
        it += pos;
        help_items_.emplace(it, key, description);
    }
}

void Window::clear_help_items()
{
    help_items_.clear();
}

void Window::draw_help_dialog()
{
    if (!show_help())
        return;

    ImGui::OpenPopup("Key Bindings");

    ImGui::SetNextWindowFocus();
    if (ImGui::BeginPopupModal("Key Bindings", nullptr,
                               ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Columns(2, "help items");
        ImGui::SetColumnWidth(0, 100 * imgui_scale_);
        ImGui::SetColumnWidth(1, 200 * imgui_scale_);
        ImGui::Separator();
        ImGui::Text("Key");
        ImGui::NextColumn();
        ImGui::Text("Description");
        ImGui::NextColumn();
        ImGui::Separator();

        for (const auto& [key, description] : help_items_)
        {
            ImGui::Text("%s", key.c_str());
            ImGui::NextColumn();
            ImGui::Text("%s", description.c_str());
            ImGui::NextColumn();
        }

        ImGui::Columns(1);
        ImGui::Separator();

        if (ImGui::Button("OK", ImVec2(300 * imgui_scale_, 0)))
        {
            show_help_ = false;
            ImGui::CloseCurrentPopup();

            // reset mouse button state and modifiers
            for (bool& b : button_)
                b = false;
            ctrl_pressed_ = shift_pressed_ = alt_pressed_ = false;
        }

        ImGui::EndPopup();
    }
}

void Window::draw_imgui()
{
    // start window
    ImGui::SetNextWindowBgAlpha(show_imgui_ ? 0.8 : 0.0);
    ImGui::SetNextWindowPos(ImVec2(4, 4), ImGuiCond_Once);
    ImGui::Begin(
        "GUI", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);

    if (show_imgui_)
    {
        // icons toolbar
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 30 * imgui_scale_);
        ImGui::BeginGroup();

        ImVec2 icon_size(35 * imgui_scale_, 35 * imgui_scale_);
        float icon_spacing = 20 * imgui_scale_;

        if (ImGui::Button(ICON_FA_XMARK, icon_size))
        {
            show_imgui_ = false;
        }

        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + icon_spacing);

        if (color_mode() == LightMode)
        {
            if (ImGui::Button(ICON_FA_MOON, icon_size))
                color_mode(DarkMode);
        }
        else
        {
            if (ImGui::Button(ICON_FA_SUN, icon_size))
                color_mode(LightMode);
        }

        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + icon_spacing);

        if (is_fullscreen())
        {
            if (ImGui::Button(ICON_FA_DOWN_LEFT_AND_UP_RIGHT_TO_CENTER,
                              icon_size))
                exit_fullscreen();
        }
        else
        {
            if (ImGui::Button(ICON_FA_UP_RIGHT_AND_DOWN_LEFT_FROM_CENTER,
                              icon_size))
                enter_fullscreen();
        }

        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + icon_spacing);

        if (ImGui::Button(ICON_FA_QUESTION, icon_size))
        {
            show_help(true);
        }

        ImGui::EndGroup();
        ImGui::PopStyleVar();

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // application's GUI
        process_imgui();

        // show help window?
        if (show_help())
            draw_help_dialog();
    }
    else
    {
        ImVec2 icon_size(35 * imgui_scale_, 35 * imgui_scale_);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 30 * imgui_scale_);
        if (ImGui::Button(ICON_FA_BARS, icon_size))
        {
            show_imgui_ = true;
        }
        ImGui::PopStyleVar();
    }

    ImGui::End();
}

int Window::run()
{
#if __EMSCRIPTEN__
    emscripten_set_main_loop(Window::render_frame_, 0, 1);
#else
    while (!glfwWindowShouldClose(window_))
    {
        Window::render_frame_();
    }
#endif
    return EXIT_SUCCESS;
}

void Window::render_frame()
{
    glfwMakeContextCurrent(window_);

#if __EMSCRIPTEN__
    // dynamicall adjust window size based on container
    double dw, dh;
    emscripten_get_element_css_size("#canvas_container", &dw, &dh);
    glfwSetWindowSize(window_, (int)dw, (int)dh);
#endif

    // do some computations
    do_processing();

    // (re)determine scaling
    int window_width, window_height, framebuffer_width, framebuffer_height;
    glfwGetWindowSize(window_, &window_width, &window_height);
    glfwGetFramebufferSize(instance_->window_, &framebuffer_width,
                           &framebuffer_height);
    scaling_ = static_cast<float>(framebuffer_width) /
               static_cast<float>(window_width);

    // setup viewport
    glViewport(0, 0, framebuffer_width, framebuffer_height);

    // clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(clear_color_[0], clear_color_[1], clear_color_[2], 0.0);

    // draw scene
    display();

    // draw GUI
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    draw_imgui();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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
    glfwSwapBuffers(window_);

    // handle events
    glfwPollEvents();
}

void Window::glfw_error(int error, const char* description)
{
    std::cerr << "error (" << error << "):" << description << std::endl;
}

void Window::glfw_character(GLFWwindow* window, unsigned int c)
{
    ImGui_ImplGlfw_CharCallback(window, c);
    if (!ImGui::GetIO().WantCaptureKeyboard)
    {
        instance_->character(c);
    }
}

void Window::glfw_keyboard(GLFWwindow* window, int key, int scancode,
                           int action, int mods)
{
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);

    // remember modifier status
    switch (key)
    {
        case GLFW_KEY_LEFT_CONTROL:
        case GLFW_KEY_RIGHT_CONTROL:
            instance_->ctrl_pressed_ = (action != GLFW_RELEASE);
            break;
        case GLFW_KEY_LEFT_SHIFT:
        case GLFW_KEY_RIGHT_SHIFT:
            instance_->shift_pressed_ = (action != GLFW_RELEASE);
            break;
        case GLFW_KEY_LEFT_ALT:
        case GLFW_KEY_RIGHT_ALT:
            instance_->alt_pressed_ = (action != GLFW_RELEASE);
            break;
    }

    if (!ImGui::GetIO().WantCaptureKeyboard)
    {
        // send event to window
        instance_->keyboard(key, scancode, action, mods);
    }
}

void Window::character(unsigned int c)
{
    if (c == 63) // question mark
    {
        show_help_ = true;
        show_imgui_ = true;
    }
}

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
        }

        case GLFW_KEY_PRINT_SCREEN:
        case GLFW_KEY_P:
        {
            screenshot();
            break;
        }
#endif
        case GLFW_KEY_F:
        {
            if (!is_fullscreen())
                enter_fullscreen();
            else
                exit_fullscreen();
            break;
        }

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

bool Window::is_fullscreen() const
{
#ifdef __EMSCRIPTEN__
    EmscriptenFullscreenChangeEvent fsce;
    emscripten_get_fullscreen_status(&fsce);
    return fsce.isFullscreen;
#else
    return glfwGetWindowMonitor(window_) != nullptr;
#endif
}

void Window::enter_fullscreen()
{
#ifdef __EMSCRIPTEN__
    emscripten_request_fullscreen("#canvas_container", false);
#else
    // get monitor
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();

    // get resolution
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    // remember window position and size
    glfwGetWindowPos(window_, &backup_xpos_, &backup_ypos_);
    glfwGetWindowSize(window_, &backup_width_, &backup_height_);

    // switch to fullscreen on primary monitor
    glfwSetWindowMonitor(window_, monitor, 0, 0, mode->width, mode->height,
                         GLFW_DONT_CARE);
#endif
}

void Window::exit_fullscreen()
{
#ifdef __EMSCRIPTEN__
    emscripten_exit_fullscreen();
#else
    glfwSetWindowMonitor(window_, nullptr, backup_xpos_, backup_ypos_,
                         backup_width_, backup_height_, GLFW_DONT_CARE);
#endif
}

void Window::glfw_motion(GLFWwindow* window, double xpos, double ypos)
{
    ImGui_ImplGlfw_CursorPosCallback(window, instance_->scaling_ * xpos,
                                     instance_->scaling_ * ypos);

    if (!ImGui::GetIO().WantCaptureMouse)
    {
        // correct for highDPI scaling
        instance_->motion(instance_->scaling_ * xpos,
                          instance_->scaling_ * ypos);
    }
}

void Window::glfw_mouse(GLFWwindow* window, int button, int action, int mods)
{
// #ifdef __EMSCRIPTEN__
    // since touch input does not trigger hover events,
    // we have to set mouse position before button press.
    // since we cannot distinguish mouse and touch events,
    // we simply do this all the time.
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    ImGui_ImplGlfw_CursorPosCallback(window, x, y);
// #endif

    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);

    instance_->button_[button] = (action == GLFW_PRESS);
    if (!ImGui::GetIO().WantCaptureMouse)
    {
        instance_->mouse(button, action, mods);
    }
}

void Window::glfw_scroll(GLFWwindow* window, double xoffset, double yoffset)
{
#ifdef __EMSCRIPTEN__
    yoffset = -yoffset;

    // thresholding for cross-browser handling
    const float t = 1;
    if (yoffset > t)
        yoffset = t;
    else if (yoffset < -t)
        yoffset = -t;
#endif

    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
    if (!ImGui::GetIO().WantCaptureMouse)
    {
        instance_->scroll(xoffset, yoffset);
    }
}

void Window::glfw_resize(GLFWwindow* /*window*/, int width, int height)
{
    instance_->width_ = width;
    instance_->height_ = height;
    instance_->resize(width, height);
}

void Window::glfw_drop(GLFWwindow* /*window*/, int count, const char** paths)
{
    instance_->drop(count, paths);
}

void Window::glfw_scale(GLFWwindow* /*window*/, float xscale, float yscale)
{
    instance_->scaling_ = std::max(1.0, 0.5 * (xscale + yscale));
}

void Window::cursor_pos(double& x, double& y) const
{
    glfwGetCursorPos(window_, &x, &y);
    x *= instance_->scaling_;
    y *= instance_->scaling_;
}

void Window::screenshot()
{
    std::stringstream filename{""};
    filename << title_ << std::to_string(screenshot_number_++) << ".png";
    std::cout << "Save screenshot to " << filename.str() << std::endl;

    // allocate buffer
    auto data = new unsigned char[3 * width_ * height_];

    // read framebuffer
    glfwMakeContextCurrent(window_);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, width_, height_, GL_RGB, GL_UNSIGNED_BYTE, data);

    // write to file
    stbi_flip_vertically_on_write(true);
    stbi_write_png(filename.str().c_str(), width_, height_, 3, data,
                   3 * width_);

    // clean up
    delete[] data;
}

#if __EMSCRIPTEN__

EM_BOOL Window::emscripten_touchstart(int, const EmscriptenTouchEvent* evt,
                                      void*)
{
    instance_->touchstart(evt);
    return EM_TRUE;
}

EM_BOOL Window::emscripten_touchmove(int, const EmscriptenTouchEvent* evt,
                                     void*)
{
    instance_->touchmove(evt);
    return EM_TRUE;
}

EM_BOOL Window::emscripten_touchend(int, const EmscriptenTouchEvent* evt, void*)
{
    instance_->touchend(evt);
    return EM_TRUE;
}

extern "C" {
EMSCRIPTEN_KEEPALIVE void light_mode()
{
    pmp::Window::light_mode();
    std::cerr << "light mode\n";
}

EMSCRIPTEN_KEEPALIVE void dark_mode()
{
    pmp::Window::dark_mode();
    std::cerr << "dark mode\n";
}
}
#endif

} // namespace pmp
