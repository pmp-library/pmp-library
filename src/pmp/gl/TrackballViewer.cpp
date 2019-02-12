//=============================================================================
// Copyright (C) 2011-2019 The pmp-library developers
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

#include "TrackballViewer.h"
#include <algorithm>

//=============================================================================

namespace pmp {

//=============================================================================

TrackballViewer::TrackballViewer(const char* title, int width, int height,
                                 bool showgui)
    : Window(title, width, height, showgui)
{
    // init mouse buttons
    for (bool& i : button_down_)
        i = false;
    wheel_pos_ = 0;

    // define basic draw modes
    add_draw_mode("Wireframe");
    add_draw_mode("Solid Flat");
    add_draw_mode("Solid Smooth");
    set_draw_mode("Solid Smooth");

    // init OpenGL state
    init();
}

//-----------------------------------------------------------------------------

TrackballViewer::~TrackballViewer() = default;

//-----------------------------------------------------------------------------

void TrackballViewer::clear_draw_modes()
{
    n_draw_modes_ = 0;
    draw_mode_names_.clear();
}

//-----------------------------------------------------------------------------

unsigned int TrackballViewer::add_draw_mode(const std::string& s)
{
    ++n_draw_modes_;
    draw_mode_names_.push_back(s);

    return n_draw_modes_ - 1;
}

//-----------------------------------------------------------------------------

void TrackballViewer::set_draw_mode(const std::string& s)
{
    for (unsigned int i = 0; i < draw_mode_names_.size(); ++i)
    {
        if (draw_mode_names_[i] == s)
        {
            draw_mode_ = i;
            break;
        }
    }
}

//-----------------------------------------------------------------------------

void TrackballViewer::keyboard(int key, int /*code*/, int action, int /*mods*/)
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

        case GLFW_KEY_SPACE:
        {
            if (++draw_mode_ >= n_draw_modes_)
                draw_mode_ = 0;
            std::string mode = draw_mode_names_[draw_mode_];
            std::cout << "setting draw mode to " << mode << std::endl;
            set_draw_mode(mode);
            break;
        }

        case GLFW_KEY_LEFT:
        {
            rotate(vec3(0, 1, 0), -5.0);
            break;
        }
        case GLFW_KEY_RIGHT:
        {
            rotate(vec3(0, 1, 0), 5.0);
            break;
        }
        case GLFW_KEY_UP:
        {
            rotate(vec3(1, 0, 0), -5.0);
            break;
        }
        case GLFW_KEY_DOWN:
        {
            rotate(vec3(1, 0, 0), 5.0);
            break;
        }
    }
}

//-----------------------------------------------------------------------------

void TrackballViewer::resize(int width, int height)
{
    width_ = width;
    height_ = height;

    glViewport(0, 0, width, height);
}

//-----------------------------------------------------------------------------

void TrackballViewer::display()
{
    // clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // adjust clipping planes to tightly fit bounding sphere
    vec4 mc(center_, 1.0);
    vec4 ec = modelview_matrix_ * mc;
    float z = -ec[2];
    near_ = 0.01 * radius_;
    far_ = 10.0 * radius_;
    fovy_ = 45.0;
    near_ = std::max(0.001f * radius_, z - radius_);
    far_ = std::max(0.002f * radius_, z + radius_);

    // update projection matrix
    projection_matrix_ = perspective_matrix(
        fovy_, (float)width_ / (float)height_, near_, far_);

    // draw the scene in current draw mode
    if (draw_mode_ < draw_mode_names_.size())
        draw(draw_mode_names_[draw_mode_]);
    else
        draw("");
}

//-----------------------------------------------------------------------------

void TrackballViewer::mouse(int button, int action, int mods)
{
    // record current modifier keys
    modifiers_ = mods;

    // mouse press
    if (action == GLFW_PRESS)
    {
        last_point_ok_ = map_to_sphere(last_point_2d_, last_point_3d_);
        button_down_[button] = true;

        // set rotation center
        if (modifiers_ == GLFW_MOD_CONTROL)
        {
            double x, y;
            cursor_pos(x, y);
            fly_to(x, y);
        }
    }

    // mouse release
    else
    {
        last_point_ok_ = false;
        button_down_[button] = false;
    }
}

//-----------------------------------------------------------------------------

void TrackballViewer::scroll(double /*xoffset*/, double yoffset)
{
    wheel_pos_ += yoffset;
    float d = -(float)yoffset * 0.12 * radius_;
#ifdef __EMSCRIPTEN__
    d *= 0.5; // scrolling in browser is faster
#endif
    translate(vec3(0.0, 0.0, d));
}

//-----------------------------------------------------------------------------

void TrackballViewer::motion(double xpos, double ypos)
{
    // zoom
    if ((button_down_[GLFW_MOUSE_BUTTON_MIDDLE]) ||
        (button_down_[GLFW_MOUSE_BUTTON_LEFT] &&
         (modifiers_ == GLFW_MOD_SHIFT)))
    {
        zoom(xpos, ypos);
    }

    // translation
    else if (button_down_[GLFW_MOUSE_BUTTON_RIGHT] ||
             (button_down_[GLFW_MOUSE_BUTTON_LEFT] &&
              (modifiers_ == GLFW_MOD_ALT)))
    {
        translation(xpos, ypos);
    }

    // rotation
    else if (button_down_[GLFW_MOUSE_BUTTON_LEFT])
    {
        rotation(xpos, ypos);
    }

    // remember points
    last_point_2d_ = ivec2(xpos, ypos);
    last_point_ok_ = map_to_sphere(last_point_2d_, last_point_3d_);
}

//-----------------------------------------------------------------------------

void TrackballViewer::init()
{
    // set initial state
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_DEPTH_TEST);

    // init modelview
    modelview_matrix_ = mat4::identity();

// turn on multi-sampling to anti-alias lines
#ifndef __EMSCRIPTEN__
    glEnable(GL_MULTISAMPLE);
    GLint n_samples;
    glGetIntegerv(GL_SAMPLES, &n_samples);
    std::cout << "Multi-sampling uses " << n_samples << " per pixel\n";
#endif
}

//-----------------------------------------------------------------------------

void TrackballViewer::set_scene(const vec3& center, float radius)
{
    center_ = center;
    radius_ = radius;
    view_all();
}

//-----------------------------------------------------------------------------

void TrackballViewer::view_all()
{
    vec4 c = vec4(center_, 1.0);
    vec4 t = modelview_matrix_ * c;
    translate(vec3(-t[0], -t[1], -t[2] - 2.5 * radius_));
}

//-----------------------------------------------------------------------------

bool TrackballViewer::pick(int x, int y, vec3& result)
{
#ifndef __EMSCRIPTEN__ // WebGL cannot read depth buffer

    // get viewport data
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    // take into accout highDPI scaling
    x *= scaling_;
    y *= scaling_;

    // in OpenGL y=0 is at the 'bottom'
    y = viewport[3] - y;

    // read depth buffer value at (x, y_new)
    float zf;
    glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &zf);

    if (zf != 1.0f)
    {
        float xf =
            ((float)x - (float)viewport[0]) / ((float)viewport[2]) * 2.0f -
            1.0f;
        float yf =
            ((float)y - (float)viewport[1]) / ((float)viewport[3]) * 2.0f -
            1.0f;
        zf = zf * 2.0f - 1.0f;

        mat4 mvp = projection_matrix_ * modelview_matrix_;
        mat4 inv = inverse(mvp);
        vec4 p = inv * vec4(xf, yf, zf, 1.0f);
        p /= p[3];

        result = vec3(p[0], p[1], p[2]);

        return true;
    }

#endif

    return false;
}

//-----------------------------------------------------------------------------

void TrackballViewer::fly_to(int x, int y)
{
    vec3 p;
    if (pick(x, y, p))
    {
        center_ = p;
        vec4 c = vec4(center_, 1.0);
        vec4 t = modelview_matrix_ * c;
        translate(vec3(-t[0], -t[1], -0.5 * t[2]));
    }
}

//-----------------------------------------------------------------------------

bool TrackballViewer::map_to_sphere(const ivec2& point2D, vec3& result)
{
    if ((point2D[0] >= 0) && (point2D[0] <= width_) && (point2D[1] >= 0) &&
        (point2D[1] <= height_))
    {
        double x = (double)(point2D[0] - 0.5 * width_) / (double)width_;
        double y = (double)(0.5 * height_ - point2D[1]) / (double)height_;
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

void TrackballViewer::rotation(int x, int y)
{
    if (last_point_ok_)
    {
        ivec2 newPoint2D;
        vec3 newPoint3D;
        bool newPointok;

        newPoint2D = ivec2(x, y);
        newPointok = map_to_sphere(newPoint2D, newPoint3D);

        if (newPointok)
        {
            vec3 axis = cross(last_point_3d_, newPoint3D);
            float cosAngle = dot(last_point_3d_, newPoint3D);

            if (fabs(cosAngle) < 1.0)
            {
                float angle = 2.0 * acos(cosAngle) * 180.0 / M_PI;
                rotate(axis, angle);
            }
        }
    }
}

//-----------------------------------------------------------------------------

void TrackballViewer::translation(int x, int y)
{
    float dx = x - last_point_2d_[0];
    float dy = y - last_point_2d_[1];

    vec4 mc = vec4(center_, 1.0);
    vec4 ec = modelview_matrix_ * mc;
    float z = -(ec[2] / ec[3]);

    float aspect = (float)width_ / (float)height_;
    float up = tan(fovy_ / 2.0f * M_PI / 180.f) * near_;
    float right = aspect * up;

    translate(vec3(2.0 * dx / width_ * right / near_ * z,
                   -2.0 * dy / height_ * up / near_ * z, 0.0f));
}

//-----------------------------------------------------------------------------

void TrackballViewer::zoom(int, int y)
{
    float dy = y - last_point_2d_[1];
    float h = height_;
    translate(vec3(0.0, 0.0, radius_ * dy * 3.0 / h));
}

//-----------------------------------------------------------------------------

void TrackballViewer::translate(const vec3& t)
{
    modelview_matrix_ = translation_matrix(t) * modelview_matrix_;
}

//-----------------------------------------------------------------------------

void TrackballViewer::rotate(const vec3& axis, float angle)
{
    // center in eye coordinates
    vec4 mc = vec4(center_, 1.0);
    vec4 ec = modelview_matrix_ * mc;
    vec3 c(ec[0] / ec[3], ec[1] / ec[3], ec[2] / ec[3]);

    modelview_matrix_ = translation_matrix(c) * rotation_matrix(axis, angle) *
                        translation_matrix(-c) * modelview_matrix_;
}

//=============================================================================
} // namespace pmp
//=============================================================================
