// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include <string>
#include <vector>

#include "pmp/visualization/Window.h"
#include "pmp/MatVec.h"

namespace pmp {

//! A simple GLFW viewer with trackball user interface
//! \ingroup visualization
class TrackballViewer : public Window
{
public:
    //! constructor
    TrackballViewer(const char* title, int width, int height,
                    bool showgui = true);

    //! destructor
    virtual ~TrackballViewer();

    //! define the center and radius of the scene/
    //! used for trackball rotation
    void set_scene(const vec3& center, float radius);

    //! adjust camera such that the whole scene (defined by set_scene()) is visible
    void view_all();

protected:
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

protected:
    //! reset the list of draw modes
    void clear_draw_modes();

    //! add a draw mode
    unsigned int add_draw_mode(const std::string& drawMode);

    //! activate a draw mode
    void set_draw_mode(const std::string& drawMode);

protected:
    //! initialize all OpenGL states
    virtual void init();

    //! this function is responsible for rendering the scene
    virtual void draw(const std::string& drawMode) = 0;

protected:
    //! turn a mouse event into a rotation around the scene center. calls rotate().
    void rotation(int x, int y);

    //! turn a mouse event into a translation in the view plane. calls translate().
    void translation(int x, int y);

    //! turn a mouse event into a zoom, i.e., translation in z-direction. calls translate().
    void zoom(int x, int y);

    //! get 3D position under the mouse cursor
    bool pick(vec3& result);

    //! get 3D position of 2D position (x,y)
    bool pick(int x, int y, vec3& result);

    //! fly toward the position Distributed under the mouse cursor and set rotation center to it
    void fly_to(int x, int y);

    //! translate the scene and update modelview matrix
    void translate(const vec3& trans);

    //! rotate the scene (around its center) and update modelview matrix
    void rotate(const vec3& axis, float angle);

    //! virtual trackball: map 2D screen point to unit sphere. used by rotate().
    bool map_to_sphere(const ivec2& point, vec3& result);

protected:
    //! draw modes
    unsigned int draw_mode_;
    unsigned int n_draw_modes_;
    std::vector<std::string> draw_mode_names_;

    //! scene position and dimension
    vec3 center_;
    float radius_;

    //! projection parameters
    float near_, far_, fovy_;

    //! OpenGL matrices
    mat4 projection_matrix_;
    mat4 modelview_matrix_;

    //! trackball helpers
    ivec2 last_point_2d_;
    vec3 last_point_3d_;
    bool last_point_ok_;
};

} // namespace pmp
