// Copyright 2011-2019 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "trackball_viewer.h"
#include "pmp/stop_watch.h"
#include <imgui.h>
#include <algorithm>
#include <numbers>

namespace pmp {

TrackballViewer::TrackballViewer(const char* title, int width, int height,
                                 bool showgui)
    : Window(title, width, height, showgui)
{
    // define basic draw modes
    add_draw_mode("Wireframe");
    add_draw_mode("Solid Flat");
    add_draw_mode("Solid Smooth");
    set_draw_mode("Solid Smooth");

    // init OpenGL state
    init();

    // add imgui help items
    add_help_item("Left/Right", "Rotate model horizontally", 0);
    add_help_item("Up/Down", "Rotate model vertically", 1);
    add_help_item("Space", "Cycle through draw modes", 2);
}

TrackballViewer::~TrackballViewer() = default;

void TrackballViewer::clear_draw_modes()
{
    n_draw_modes_ = 0;
    draw_mode_names_.clear();
}

unsigned int TrackballViewer::add_draw_mode(const std::string& s)
{
    ++n_draw_modes_;
    draw_mode_names_.push_back(s);

    return n_draw_modes_ - 1;
}

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

void TrackballViewer::keyboard(int key, int code, int action, int mods)
{
    if (action != GLFW_PRESS && action != GLFW_REPEAT)
        return;

    switch (key)
    {
        case GLFW_KEY_SPACE:
        {
            if (++draw_mode_ >= n_draw_modes_)
                draw_mode_ = 0;
            const std::string mode = draw_mode_names_[draw_mode_];
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
        // case GLFW_KEY_F:
        // {
        //     double fps = measure_fps();
        //     std::cout << "FPS: " << fps << std::endl;
        //     break;
        // }

        default:
        {
            Window::keyboard(key, code, action, mods);
            break;
        }
    }
}

void TrackballViewer::display()
{
    // adjust clipping planes to tightly fit bounding sphere
    const vec4 mc(center_, 1.0);
    vec4 ec = modelview_matrix_ * mc;
    const float z = -ec[2];
    near_ = 0.01 * radius_;
    far_ = 10.0 * radius_;
    fovy_ = 45.0;
    near_ = std::max(0.001f * radius_, z - radius_);
    far_ = std::max(0.002f * radius_, z + radius_);

    // update projection matrix
    projection_matrix_ = perspective_matrix(
        fovy_, (float)width() / (float)height(), near_, far_);

    // draw the scene in current draw mode
    if (draw_mode_ < draw_mode_names_.size())
        draw(draw_mode_names_[draw_mode_]);
    else
        draw("");
}

void TrackballViewer::mouse(int /*button*/, int action, int /*mods*/)
{
    // mouse press
    if (action == GLFW_PRESS)
    {
        // set rotation center
        if (ctrl_pressed())
        {
            double x, y;
            cursor_pos(x, y);
            fly_to(x, y);
        }
    }

    // force re-initialization of rotation/translation/zoom
    prev_point_ok_ = false;
}

void TrackballViewer::scroll(double /*xoffset*/, double yoffset)
{
    float d = (float)yoffset * 0.12 * radius_; // NOLINT
#ifdef __EMSCRIPTEN__
    d *= 0.5; // scrolling in browser is faster
    d = -d;   // invert direction
#endif
    translate(vec3(0.0, 0.0, d));
}

void TrackballViewer::motion(double xpos, double ypos)
{
#ifdef __EMSCRIPTEN__
    if (num_touches_ > 1) return;
#endif

    if (prev_point_ok_)
    {
        // zoom
        if (right_mouse_pressed() || (left_mouse_pressed() && shift_pressed()))
        {
            zoom(xpos, ypos);
        }

        // translation
        else if (middle_mouse_pressed() ||
                 (left_mouse_pressed() && alt_pressed()))
        {
            translation(xpos, ypos);
        }

        // rotation
        else if (left_mouse_pressed())
        {
            rotation(xpos, ypos);
        }
    }

    // remember points
    prev_point_2d_ = ivec2(xpos, ypos);
    prev_point_ok_ = map_to_sphere(prev_point_2d_, prev_point_3d_);
}

void TrackballViewer::init()
{
    // set initial state
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CCW);

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

void TrackballViewer::set_scene(const vec3& center, float radius)
{
    center_ = center;
    radius_ = radius;
    view_all();
}

void TrackballViewer::view_all()
{
    const vec4 c = vec4(center_, 1.0);
    vec4 t = modelview_matrix_ * c;
    translate(vec3(-t[0], -t[1], -t[2] - 2.5 * radius_));
}

bool TrackballViewer::pick(vec3& result)
{
    double x, y;
    cursor_pos(x, y);
    return pick(x, y, result);
}

bool TrackballViewer::pick(int x, int y, vec3& result)
{
#ifndef __EMSCRIPTEN__ // WebGL cannot read depth buffer

    // get viewport data
    std::array<GLint, 4> viewport;
    glGetIntegerv(GL_VIEWPORT, viewport.data());

    // in OpenGL y=0 is at the 'bottom'
    y = viewport[3] - y;

    // read depth buffer value at (x, y_new)
    float zf;
    glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &zf);

    if (zf != 1.0f)
    {
        const float xf =
            ((float)x - (float)viewport[0]) / ((float)viewport[2]) * 2.0f -
            1.0f;
        const float yf =
            ((float)y - (float)viewport[1]) / ((float)viewport[3]) * 2.0f -
            1.0f;
        zf = zf * 2.0f - 1.0f;

        const mat4 mvp = projection_matrix_ * modelview_matrix_;
        const mat4 inv = inverse(mvp);
        vec4 p = inv * vec4(xf, yf, zf, 1.0f);
        p /= p[3];

        result = vec3(p[0], p[1], p[2]);

        return true;
    }

#endif

    return false;
}

void TrackballViewer::fly_to(int x, int y)
{
    vec3 p;
    if (pick(x, y, p))
    {
        center_ = p;
        const vec4 c = vec4(center_, 1.0);
        vec4 t = modelview_matrix_ * c;
        translate(vec3(-t[0], -t[1], -0.5 * t[2]));
    }
}

bool TrackballViewer::map_to_sphere(const ivec2& point2D, vec3& result)
{
    if ((point2D[0] >= 0) && (point2D[0] <= width()) && (point2D[1] >= 0) &&
        (point2D[1] <= height()))
    {
        const double w = width();
        const double h = height();
        const double x = (double)(point2D[0] - 0.5 * w) / w;
        const double y = (double)(0.5 * h - point2D[1]) / h;
        const double sinx = sin(std::numbers::pi * x * 0.5);
        const double siny = sin(std::numbers::pi * y * 0.5);
        const double sinx2siny2 = sinx * sinx + siny * siny;

        result[0] = sinx;
        result[1] = siny;
        result[2] = sinx2siny2 < 1.0 ? sqrt(1.0 - sinx2siny2) : 0.0;

        return true;
    }
    else
        return false;
}

void TrackballViewer::rotation(int x, int y)
{
    if (prev_point_ok_)
    {
        ivec2 new_point2d;
        vec3 new_point3d;
        bool new_point_ok;

        new_point2d = ivec2(x, y);
        new_point_ok = map_to_sphere(new_point2d, new_point3d);

        if (new_point_ok)
        {
            const vec3 axis = cross(prev_point_3d_, new_point3d);
            const float cos_angle = dot(prev_point_3d_, new_point3d);

            if (fabs(cos_angle) < 1.0)
            {
                const float angle =
                    2.0 * acos(cos_angle) * 180.0 / std::numbers::pi;
                rotate(axis, angle);
            }
        }
    }
}

void TrackballViewer::translation(int x, int y)
{
    const float dx = x - prev_point_2d_[0];
    const float dy = y - prev_point_2d_[1];

    const vec4 mc = vec4(center_, 1.0);
    vec4 ec = modelview_matrix_ * mc;
    const float z = -(ec[2] / ec[3]);

    const float aspect = (float)width() / (float)height();
    const float up = tan(fovy_ / 2.0f * std::numbers::pi / 180.f) * near_;
    const float right = aspect * up;

    translate(vec3(2.0 * dx / width() * right / near_ * z,
                   -2.0 * dy / height() * up / near_ * z, 0.0f));
}

void TrackballViewer::zoom(int, int y)
{
    const float dy = y - prev_point_2d_[1];
    const float h = height();
    translate(vec3(0.0, 0.0, radius_ * dy * 3.0 / h));
}

void TrackballViewer::translate(const vec3& t)
{
    modelview_matrix_ = translation_matrix(t) * modelview_matrix_;
}

void TrackballViewer::rotate(const vec3& axis, float angle)
{
    // center in eye coordinates
    const vec4 mc = vec4(center_, 1.0);
    vec4 ec = modelview_matrix_ * mc;
    const vec3 c(ec[0] / ec[3], ec[1] / ec[3], ec[2] / ec[3]);

    modelview_matrix_ = translation_matrix(c) * rotation_matrix(axis, angle) *
                        translation_matrix(-c) * modelview_matrix_;
}

double TrackballViewer::measure_fps()
{
	double        fps(0.0);
    unsigned int  i, frames = 360;
    const float   angle = 360.0/(float)frames;
    vec3          axis;
	
    // disable vsync
    glfwSwapInterval(0);

	StopWatch timer;
	timer.start();
	
    for (i=0, axis=vec3(1,0,0); i<frames; ++i)
    { rotate(axis, angle); render_frame(); }
    for (i=0, axis=vec3(0,1,0); i<frames; ++i)
    { rotate(axis, angle); render_frame(); }
    for (i=0, axis=vec3(0,0,1); i<frames; ++i)
    { rotate(axis, angle); render_frame(); }
	
    glFinish();

	timer.stop();
	fps = (1000.0 / timer.elapsed() * (3.0 * frames));

    // re-enable vsync
    glfwSwapInterval(1);

    return fps;
}

#if __EMSCRIPTEN__

void TrackballViewer::touchstart(const EmscriptenTouchEvent* event) 
{
    num_touches_ = event->numTouches;

    // invalidate touch-based transform
    prev_point_ok_ = false;
    prev_pinch_distance_ = -1;
}

void TrackballViewer::touchmove(const EmscriptenTouchEvent* event)
{
    if (num_touches_ == 2 && !ImGui::GetIO().WantCaptureMouse)
    {
        const vec2 pos0 = high_dpi_scaling() * vec2(event->touches[0].pageX, event->touches[0].pageY);
        const vec2 pos1 = high_dpi_scaling() * vec2(event->touches[1].pageX, event->touches[1].pageY);
        const vec2 pos = 0.5*(pos0 + pos1);
        const float pinch_distance = distance(pos0, pos1);

        // pan
        if (prev_point_ok_)
        {
            translation(pos[0], pos[1]);
        }
        prev_point_2d_ = ivec2(pos[0], pos[1]);
        prev_point_ok_ = true;

        // scale
        if (prev_pinch_distance_ > 0)
        {
            const float h = height();
            translate(vec3(0.0, 0.0, radius_ * (pinch_distance-prev_pinch_distance_) * 5.0 / h));
        }
        prev_pinch_distance_ = pinch_distance;
    }
}

void TrackballViewer::touchend(const EmscriptenTouchEvent* event) 
{
    num_touches_ = event->numTouches-1;

    // invalidate touch-based transform
    prev_point_ok_ = false;
    prev_pinch_distance_ = -1;
}

#endif

} // namespace pmp
