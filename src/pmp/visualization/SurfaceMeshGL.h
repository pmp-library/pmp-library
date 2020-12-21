// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include <limits>

#include "pmp/SurfaceMesh.h"
#include "pmp/visualization/GL.h"
#include "pmp/visualization/Shader.h"
#include "pmp/MatVec.h"

namespace pmp {

//! Class for rendering surface meshes using OpenGL
//! \ingroup visualization
class SurfaceMeshGL : public SurfaceMesh
{
public:
    //! Constructor
    SurfaceMeshGL();

    //! default destructor
    ~SurfaceMeshGL();

    //! get front color
    const vec3& front_color() const { return front_color_; }
    //! set front color
    void set_front_color(const vec3& color) { front_color_ = color; }

    //! get back color
    const vec3& back_color() const { return back_color_; }
    //! set back color
    void set_back_color(const vec3& color) { back_color_ = color; }

    //! get ambient reflection coefficient
    float ambient() const { return ambient_; }
    //! set ambient reflection coefficient
    void set_ambient(float a) { ambient_ = a; }

    //! get diffuse reflection coefficient
    float diffuse() const { return diffuse_; }
    //! set diffuse reflection coefficient
    void set_diffuse(float d) { diffuse_ = d; }

    //! get specular reflection coefficient
    float specular() const { return specular_; }
    //! set specular reflection coefficient
    void set_specular(float s) { specular_ = s; }

    //! get specular shininess coefficient
    float shininess() const { return shininess_; }
    //! set specular shininess coefficient
    void set_shininess(float s) { shininess_ = s; }

    //! get alpha value for transparent rendering
    float alpha() const { return alpha_; }
    //! set alpha value for transparent rendering
    void set_alpha(float a) { alpha_ = a; }

    //! get crease angle (in degrees) for visualization of sharp edges
    Scalar crease_angle() const { return crease_angle_; }
    //! set crease angle (in degrees) for visualization of sharp edges
    void set_crease_angle(Scalar ca);

    //! \brief Control usage of color information
    //! \details Either per-vertex or per-face colors can be used. Vertex colors
    //! are only used if the mesh has a per-vertex property of type Color
    //! named \c "v:color". Face colors are only used if the mesh has a per-face
    //! property of type Color named \c "f:color". If set to false, the
    //! default front and back colors are used. Default is \c true.
    //! \note Vertex colors take precedence over face colors.
    void set_use_colors(bool use_colors) { use_colors_ = use_colors; }

    //! draw the mesh
    void draw(const mat4& projection_matrix, const mat4& modelview_matrix,
              const std::string draw_mode);

    //! update all opengl buffers for efficient core profile rendering
    void update_opengl_buffers();

    //! use color map to visualize scalar fields
    void use_cold_warm_texture();

    //! setup checkerboard texture
    void use_checkerboard_texture();

    //! load texture from file
    //! \param filename the location and name of the texture
    //! \param format internal format (GL_RGB, GL_RGBA, GL_SRGB8, etc.)
    //! \param min_filter interpolation filter for minification
    //! \param mag_filter interpolation filter for magnification
    //! \param wrap texture coordinates wrap preference
    bool load_texture(const char* filename, GLint format = GL_RGB,
                      GLint min_filter = GL_LINEAR_MIPMAP_LINEAR,
                      GLint mag_filter = GL_LINEAR,
                      GLint wrap = GL_CLAMP_TO_EDGE);

    //! Load mat-cap texture from file. The mat-cap will be used
    //! whenever the drawing mode is "Texture". This also means
    //! that you cannot have texture and mat-cap at the same time.
    //! \param filename the location and name of the texture
    //! \sa See src/apps/mview.cpp for an example usage.
    bool load_matcap(const char* filename);

private: // helpers for computing triangulation of a polygon
    struct Triangulation
    {
        Triangulation(Scalar a = std::numeric_limits<Scalar>::max(), int s = -1)
            : area(a), split(s)
        {
        }
        Scalar area;
        int split;
    };

    // table to hold triangulation data
    std::vector<Triangulation> triangulation_;

    // valence of currently triangulated polygon
    unsigned int polygon_valence_;

    // reserve n*n array for computing triangulation
    void init_triangulation(unsigned int n)
    {
        triangulation_.clear();
        triangulation_.resize(n * n);
        polygon_valence_ = n;
    }

    // access triangulation array
    Triangulation& triangulation(int start, int end)
    {
        return triangulation_[polygon_valence_ * start + end];
    }

    // compute squared area of triangle. used for triangulate().
    inline Scalar area(const vec3& p0, const vec3& p1, const vec3& p2) const
    {
        return sqrnorm(cross(p1 - p0, p2 - p0));
    }

    // triangulate a polygon such that the sum of squared triangle areas is minimized.
    // this prevents overlapping/folding triangles for non-convex polygons.
    void tesselate(const std::vector<vec3>& points,
                   std::vector<ivec3>& triangles);

private:
    //! OpenGL buffers
    GLuint vertex_array_object_;
    GLuint vertex_buffer_;
    GLuint color_buffer_;
    GLuint normal_buffer_;
    GLuint tex_coord_buffer_;
    GLuint edge_buffer_;
    GLuint feature_buffer_;

    //! buffer sizes
    GLsizei n_vertices_;
    GLsizei n_edges_;
    GLsizei n_triangles_;
    GLsizei n_features_;
    bool has_texcoords_;
    bool has_vertex_colors_;

    //! shaders
    Shader phong_shader_;
    Shader matcap_shader_;

    //! material properties
    vec3 front_color_, back_color_;
    float ambient_, diffuse_, specular_, shininess_, alpha_;
    bool srgb_;
    bool use_colors_;
    float crease_angle_;

    //! 1D texture for scalar field rendering
    GLuint texture_;
    enum TextureMode
    {
        ColdWarmTexture,
        CheckerboardTexture,
        MatCapTexture,
        OtherTexture
    } texture_mode_;
};

} // namespace pmp
