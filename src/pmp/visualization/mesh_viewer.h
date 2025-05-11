// Copyright 2011-2021 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include "pmp/visualization/trackball_viewer.h"
#include "pmp/surface_mesh.h"
#include "pmp/visualization/renderer.h"

namespace pmp {

//! Simple viewer for a SurfaceMesh
//! \ingroup visualization
class MeshViewer : public TrackballViewer
{
public:
    //! constructor
    MeshViewer(const char* title, int width, int height, bool showgui = true);

    //! destructor
    ~MeshViewer() override = default;

    //! load a mesh from file \p filename
    virtual void load_mesh(const char* filename);

    //! load a matcap texture from file \p filename
    void load_matcap(const char* filename);

    //! load a texture from file \p filename
    void load_texture(const char* filename, GLint format = GL_RGB,
                      GLint min_filter = GL_LINEAR_MIPMAP_LINEAR,
                      GLint mag_filter = GL_LINEAR,
                      GLint wrap = GL_CLAMP_TO_EDGE);

    //! update mesh normals and all buffers for OpenGL rendering.  call this
    //! function whenever you change either the vertex positions or the
    //! triangulation of the mesh
    virtual void update_mesh();

    //! draw the scene in different draw modes
    void draw(const std::string& draw_mode) override;

    //! handle ImGUI interface
    void process_imgui() override;

    //! this function handles keyboard events
    void keyboard(int key, int code, int action, int mod) override;

    //! load mesh by dropping file onto the window
    void drop(int count, const char** paths) override;

    //! get vertex closest to 3D position under the mouse cursor
    Vertex pick_vertex(int x, int y);

protected:
    SurfaceMesh mesh_;
    Renderer renderer_;
    std::string filename_; //!< the current file
    float crease_angle_;
    int point_size_;
};

} // namespace pmp
