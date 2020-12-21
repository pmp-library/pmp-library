// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/visualization/SurfaceMeshGL.h"

#include <stb_image.h>

#include "pmp/visualization/PhongShader.h"
#include "pmp/visualization/MatCapShader.h"
#include "pmp/visualization/ColdWarmTexture.h"
#include "pmp/algorithms/SurfaceNormals.h"

namespace pmp {

SurfaceMeshGL::SurfaceMeshGL()
{
    // initialize GL buffers to zero
    vertex_array_object_ = 0;
    vertex_buffer_ = 0;
    color_buffer_ = 0;
    normal_buffer_ = 0;
    tex_coord_buffer_ = 0;
    edge_buffer_ = 0;
    feature_buffer_ = 0;

    // initialize buffer sizes
    n_vertices_ = 0;
    n_edges_ = 0;
    n_triangles_ = 0;
    n_features_ = 0;
    has_texcoords_ = false;
    has_vertex_colors_ = false;

    // material parameters
    front_color_ = vec3(0.6, 0.6, 0.6);
    back_color_ = vec3(0.5, 0.0, 0.0);
    ambient_ = 0.1;
    diffuse_ = 0.8;
    specular_ = 0.6;
    shininess_ = 100.0;
    alpha_ = 1.0;
    srgb_ = false;
    use_colors_ = true;
    crease_angle_ = 180.0;

    // initialize texture
    texture_ = 0;
    texture_mode_ = OtherTexture;
}

SurfaceMeshGL::~SurfaceMeshGL()
{
    // delete OpenGL buffers
    glDeleteBuffers(1, &vertex_buffer_);
    glDeleteBuffers(1, &color_buffer_);
    glDeleteBuffers(1, &normal_buffer_);
    glDeleteBuffers(1, &tex_coord_buffer_);
    glDeleteBuffers(1, &edge_buffer_);
    glDeleteBuffers(1, &feature_buffer_);
    glDeleteVertexArrays(1, &vertex_array_object_);
    glDeleteTextures(1, &texture_);
}

bool SurfaceMeshGL::load_texture(const char* filename, GLint format,
                                 GLint min_filter, GLint mag_filter, GLint wrap)
{
#ifdef __EMSCRIPTEN__
    // emscripen/WebGL does not like mapmapping for SRGB textures
    if ((min_filter == GL_NEAREST_MIPMAP_NEAREST ||
         min_filter == GL_NEAREST_MIPMAP_LINEAR ||
         min_filter == GL_LINEAR_MIPMAP_NEAREST ||
         min_filter == GL_LINEAR_MIPMAP_LINEAR) &&
        (format == GL_SRGB8))
        min_filter = GL_LINEAR;
#endif

    // choose number of components (RGB or RGBA) based on format
    int loadComponents;
    GLint loadFormat;
    switch (format)
    {
        case GL_RGB:
        case GL_SRGB8:
            loadComponents = 3;
            loadFormat = GL_RGB;
            break;

        case GL_RGBA:
        case GL_SRGB8_ALPHA8:
            loadComponents = 4;
            loadFormat = GL_RGBA;
            break;

        default:
            loadComponents = 3;
            loadFormat = GL_RGB;
    }

    // load with stb_image
    int width, height, n;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* img =
        stbi_load(filename, &width, &height, &n, loadComponents);
    if (!img)
        return false;

    // delete old texture
    glDeleteTextures(1, &texture_);

    // setup new texture
    glGenTextures(1, &texture_);
    glBindTexture(GL_TEXTURE_2D, texture_);

    // upload texture data
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, loadFormat,
                 GL_UNSIGNED_BYTE, img);

    // compute mipmaps
    if (min_filter == GL_LINEAR_MIPMAP_LINEAR)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    // set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

    // use SRGB rendering?
    srgb_ = (format == GL_SRGB8);

    // free memory
    stbi_image_free(img);

    texture_mode_ = OtherTexture;
    return true;
}

bool SurfaceMeshGL::load_matcap(const char* filename)
{
    if (!load_texture(filename, GL_RGBA, GL_LINEAR, GL_LINEAR,
                      GL_CLAMP_TO_EDGE))
        return false;

    texture_mode_ = MatCapTexture;
    return true;
}

void SurfaceMeshGL::use_cold_warm_texture()
{
    if (texture_mode_ != ColdWarmTexture)
    {
        // delete old texture
        glDeleteTextures(1, &texture_);

        // setup new texture
        glGenTextures(1, &texture_);
        glBindTexture(GL_TEXTURE_2D, texture_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 1, 0, GL_RGB,
                     GL_UNSIGNED_BYTE, cold_warm_texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        srgb_ = false;
        texture_mode_ = ColdWarmTexture;
    }
}

void SurfaceMeshGL::use_checkerboard_texture()
{
    if (texture_mode_ != CheckerboardTexture)
    {
        // delete old texture
        glDeleteTextures(1, &texture_);

        // generate checkerboard-like image
        const unsigned int res = 512;
        auto* tex = new GLubyte[res * res * 3];
        GLubyte* tp = tex;
        for (unsigned int x = 0; x < res; ++x)
        {
            for (unsigned int y = 0; y < res; ++y)
            {
                if (((x & 0x20) == 0) ^ ((y & 0x20) == 0))
                {
                    *(tp++) = 42;
                    *(tp++) = 157;
                    *(tp++) = 223;
                }
                else
                {
                    *(tp++) = 255;
                    *(tp++) = 255;
                    *(tp++) = 255;
                }
            }
        }

        // generate texture
        glGenTextures(1, &texture_);
        glBindTexture(GL_TEXTURE_2D, texture_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, res, res, 0, GL_RGB,
                     GL_UNSIGNED_BYTE, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // clean up
        delete[] tex;

        srgb_ = false;
        texture_mode_ = CheckerboardTexture;
    }
}

void SurfaceMeshGL::set_crease_angle(Scalar ca)
{
    if (ca != crease_angle_)
    {
        crease_angle_ = std::max(Scalar(0), std::min(Scalar(180), ca));
        update_opengl_buffers();
    }
}

void SurfaceMeshGL::update_opengl_buffers()
{
    // are buffers already initialized?
    if (!vertex_array_object_)
    {
        glGenVertexArrays(1, &vertex_array_object_);
        glBindVertexArray(vertex_array_object_);
        glGenBuffers(1, &vertex_buffer_);
        glGenBuffers(1, &color_buffer_);
        glGenBuffers(1, &normal_buffer_);
        glGenBuffers(1, &tex_coord_buffer_);
        glGenBuffers(1, &edge_buffer_);
        glGenBuffers(1, &feature_buffer_);
    }

    // activate VAO
    glBindVertexArray(vertex_array_object_);

    // get properties
    auto vpos = get_vertex_property<Point>("v:point");
    auto vcolor = get_vertex_property<Color>("v:color");
    auto vtex = get_vertex_property<TexCoord>("v:tex");
    auto htex = get_halfedge_property<TexCoord>("h:tex");
    auto fcolor = get_face_property<Color>("f:color");

    // index array for remapping vertex indices during duplication
    auto vertex_indices = add_vertex_property<size_t>("v:index");

    // produce arrays of points, normals, and texcoords
    // (duplicate vertices to allow for flat shading)
    std::vector<vec3> position_array;
    std::vector<vec3> color_array;
    std::vector<vec3> normal_array;
    std::vector<vec2> tex_array;
    std::vector<ivec3> triangles;

    // we have a mesh: fill arrays by looping over faces
    if (n_faces())
    {
        // reserve memory
        position_array.reserve(3 * n_faces());
        normal_array.reserve(3 * n_faces());
        if (htex || vtex)
            tex_array.reserve(3 * n_faces());

        if ((vcolor || fcolor) && use_colors_)
            color_array.reserve(3 * n_faces());

        // precompute normals for easy cases
        FaceProperty<Normal> fnormals;
        VertexProperty<Normal> vnormals;
        if (crease_angle_ < 1)
        {
            fnormals = add_face_property<Normal>("gl:fnormal");
            for (auto f : faces())
                fnormals[f] = SurfaceNormals::compute_face_normal(*this, f);
        }
        else if (crease_angle_ > 170)
        {
            vnormals = add_vertex_property<Normal>("gl:vnormal");
            for (auto v : vertices())
                vnormals[v] = SurfaceNormals::compute_vertex_normal(*this, v);
        }

        // data per face (for all corners)
        std::vector<Halfedge> corner_halfedges;
        std::vector<Vertex> corner_vertices;
        std::vector<vec3> corner_positions;
        std::vector<vec3> corner_colors;
        std::vector<vec3> corner_normals;
        std::vector<vec2> corner_texcoords;

        // convert from degrees to radians
        const Scalar crease_angle_radians = crease_angle_ / 180.0 * M_PI;

        size_t vidx(0);

        // loop over all faces
        for (auto f : faces())
        {
            // collect corner positions and normals
            corner_halfedges.clear();
            corner_vertices.clear();
            corner_positions.clear();
            corner_colors.clear();
            corner_normals.clear();
            corner_texcoords.clear();
            Vertex v;
            Normal n;

            for (auto h : halfedges(f))
            {
                v = to_vertex(h);
                corner_halfedges.push_back(h);
                corner_vertices.push_back(v);
                corner_positions.push_back((vec3)vpos[v]);

                if (crease_angle_ < 1)
                {
                    n = fnormals[f];
                }
                else if (crease_angle_ > 170)
                {
                    n = vnormals[v];
                }
                else
                {
                    n = SurfaceNormals::compute_corner_normal(
                        *this, h, crease_angle_radians);
                }
                corner_normals.push_back((vec3)n);

                if (htex)
                {
                    corner_texcoords.push_back((vec2)htex[h]);
                }
                else if (vtex)
                {
                    corner_texcoords.push_back((vec2)vtex[v]);
                }

                if (vcolor && use_colors_)
                {
                    corner_colors.push_back((vec3)vcolor[v]);
                }
                else if (fcolor && use_colors_)
                {
                    corner_colors.push_back((vec3)fcolor[f]);
                }
            }
            assert(corner_vertices.size() >= 3);

            // tessellate face into triangles
            tesselate(corner_positions, triangles);
            for (auto& t : triangles)
            {
                int i0 = t[0];
                int i1 = t[1];
                int i2 = t[2];

                position_array.push_back(corner_positions[i0]);
                position_array.push_back(corner_positions[i1]);
                position_array.push_back(corner_positions[i2]);

                normal_array.push_back(corner_normals[i0]);
                normal_array.push_back(corner_normals[i1]);
                normal_array.push_back(corner_normals[i2]);

                if (htex || vtex)
                {
                    tex_array.push_back(corner_texcoords[i0]);
                    tex_array.push_back(corner_texcoords[i1]);
                    tex_array.push_back(corner_texcoords[i2]);
                }

                if ((vcolor || fcolor) && use_colors_)
                {
                    color_array.push_back(corner_colors[i0]);
                    color_array.push_back(corner_colors[i1]);
                    color_array.push_back(corner_colors[i2]);
                }

                vertex_indices[corner_vertices[i0]] = vidx++;
                vertex_indices[corner_vertices[i1]] = vidx++;
                vertex_indices[corner_vertices[i2]] = vidx++;
            }
        }

        // clean up
        if (vnormals)
            remove_vertex_property(vnormals);
        if (fnormals)
            remove_face_property(fnormals);
    }

    // we have a point cloud
    else if (n_vertices())
    {
        auto position = vertex_property<Point>("v:point");
        if (position)
        {
            position_array.reserve(n_vertices());
            for (auto v : vertices())
                position_array.push_back((vec3)position[v]);
        }

        auto normals = get_vertex_property<Point>("v:normal");
        if (normals)
        {
            normal_array.reserve(n_vertices());
            for (auto v : vertices())
                normal_array.push_back((vec3)normals[v]);
        }

        if (vcolor && use_colors_)
        {
            color_array.reserve(n_vertices());
            for (auto v : vertices())
                color_array.push_back((vec3)vcolor[v]);
        }
    }

    // upload vertices
    if (!position_array.empty())
    {
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
        glBufferData(GL_ARRAY_BUFFER, position_array.size() * 3 * sizeof(float),
                     position_array.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);
        n_vertices_ = position_array.size();
    }
    else
        n_vertices_ = 0;

    // upload normals
    if (!normal_array.empty())
    {
        glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_);
        glBufferData(GL_ARRAY_BUFFER, normal_array.size() * 3 * sizeof(float),
                     normal_array.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(1);
    }

    // upload texture coordinates
    if (!tex_array.empty())
    {
        glBindBuffer(GL_ARRAY_BUFFER, tex_coord_buffer_);
        glBufferData(GL_ARRAY_BUFFER, tex_array.size() * 2 * sizeof(float),
                     tex_array.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(2);
        has_texcoords_ = true;
    }
    else
        has_texcoords_ = false;

    // upload colors of vertices
    if (!color_array.empty())
    {
        glBindBuffer(GL_ARRAY_BUFFER, color_buffer_);
        glBufferData(GL_ARRAY_BUFFER, color_array.size() * 3 * sizeof(float),
                     color_array.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(3);
        has_vertex_colors_ = true;
    }
    else
        has_vertex_colors_ = false;

    // edge indices
    if (n_edges())
    {
        std::vector<unsigned int> edgeArray;
        edgeArray.reserve(n_edges());
        for (auto e : edges())
        {
            edgeArray.push_back(vertex_indices[vertex(e, 0)]);
            edgeArray.push_back(vertex_indices[vertex(e, 1)]);
        }
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, edge_buffer_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     edgeArray.size() * sizeof(unsigned int), edgeArray.data(),
                     GL_STATIC_DRAW);
        n_edges_ = edgeArray.size();
    }
    else
        n_edges_ = 0;

    // feature edges
    auto efeature = get_edge_property<bool>("e:feature");
    if (efeature)
    {
        std::vector<unsigned int> features;

        for (auto e : edges())
        {
            if (efeature[e])
            {
                features.push_back(vertex_indices[vertex(e, 0)]);
                features.push_back(vertex_indices[vertex(e, 1)]);
            }
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, feature_buffer_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     features.size() * sizeof(unsigned int), features.data(),
                     GL_STATIC_DRAW);
        n_features_ = features.size();
    }
    else
        n_features_ = 0;

    // unbind vertex arry
    glBindVertexArray(0);

    // remove vertex index property again
    remove_vertex_property(vertex_indices);
}

void SurfaceMeshGL::draw(const mat4& projection_matrix,
                         const mat4& modelview_matrix,
                         const std::string draw_mode)
{
    // did we generate buffers already?
    if (!vertex_array_object_)
    {
        update_opengl_buffers();
    }

    // load shader?
    if (!phong_shader_.is_valid())
    {
        if (!phong_shader_.source(phong_vshader, phong_fshader))
            exit(1);
    }

    // load shader?
    if (!matcap_shader_.is_valid())
    {
        if (!matcap_shader_.source(matcap_vshader, matcap_fshader))
            exit(1);
    }

    // we need some texture, otherwise WebGL complains
    if (!texture_)
    {
        use_cold_warm_texture();
    }

    // empty mesh?
    if (is_empty())
        return;

    // allow for transparent objects
    glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);

    // setup matrices
    mat4 mv_matrix = modelview_matrix;
    mat4 mvp_matrix = projection_matrix * modelview_matrix;
    mat3 n_matrix = inverse(transpose(linear_part(mv_matrix)));

    // setup shader
    phong_shader_.use();
    phong_shader_.set_uniform("modelview_projection_matrix", mvp_matrix);
    phong_shader_.set_uniform("modelview_matrix", mv_matrix);
    phong_shader_.set_uniform("normal_matrix", n_matrix);
    phong_shader_.set_uniform("point_size", 5.0f);
    phong_shader_.set_uniform("light1", vec3(1.0, 1.0, 1.0));
    phong_shader_.set_uniform("light2", vec3(-1.0, 1.0, 1.0));
    phong_shader_.set_uniform("front_color", front_color_);
    phong_shader_.set_uniform("back_color", back_color_);
    phong_shader_.set_uniform("ambient", ambient_);
    phong_shader_.set_uniform("diffuse", diffuse_);
    phong_shader_.set_uniform("specular", specular_);
    phong_shader_.set_uniform("shininess", shininess_);
    phong_shader_.set_uniform("alpha", alpha_);
    phong_shader_.set_uniform("use_lighting", true);
    phong_shader_.set_uniform("use_texture", false);
    phong_shader_.set_uniform("use_srgb", false);
    phong_shader_.set_uniform("show_texture_layout", false);
    phong_shader_.set_uniform("use_vertex_color", has_vertex_colors_);

    glBindVertexArray(vertex_array_object_);

    if (draw_mode == "Points")
    {
#ifndef __EMSCRIPTEN__
        glEnable(GL_PROGRAM_POINT_SIZE);
#endif
        glDrawArrays(GL_POINTS, 0, n_vertices_);
    }

    else if (draw_mode == "Hidden Line")
    {
        if (n_faces())
        {
            // draw faces
            glDepthRange(0.01, 1.0);
            glDrawArrays(GL_TRIANGLES, 0, n_vertices_);
            glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);

            // overlay edges
            glDepthRange(0.0, 1.0);
            glDepthFunc(GL_LEQUAL);
            phong_shader_.set_uniform("front_color", vec3(0.1, 0.1, 0.1));
            phong_shader_.set_uniform("back_color", vec3(0.1, 0.1, 0.1));
            phong_shader_.set_uniform("use_lighting", false);
            phong_shader_.set_uniform("use_vertex_color", false);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, edge_buffer_);
            glDrawElements(GL_LINES, n_edges_, GL_UNSIGNED_INT, nullptr);
            glDepthFunc(GL_LESS);
        }
    }

    else if (draw_mode == "Smooth Shading")
    {
        if (n_faces())
        {
            glDrawArrays(GL_TRIANGLES, 0, n_vertices_);
        }
    }

    else if (draw_mode == "Texture")
    {
        if (n_faces())
        {
            if (texture_mode_ == MatCapTexture)
            {
                matcap_shader_.use();
                matcap_shader_.set_uniform("modelview_projection_matrix",
                                           mvp_matrix);
                matcap_shader_.set_uniform("normal_matrix", n_matrix);
                matcap_shader_.set_uniform("alpha", alpha_);
                glBindTexture(GL_TEXTURE_2D, texture_);
                glDrawArrays(GL_TRIANGLES, 0, n_vertices_);
            }
            else
            {
                phong_shader_.set_uniform("front_color", vec3(0.9, 0.9, 0.9));
                phong_shader_.set_uniform("back_color", vec3(0.3, 0.3, 0.3));
                phong_shader_.set_uniform("use_texture", true);
                phong_shader_.set_uniform("use_vertex_color", false);
                phong_shader_.set_uniform("use_srgb", srgb_);
                glBindTexture(GL_TEXTURE_2D, texture_);
                glDrawArrays(GL_TRIANGLES, 0, n_vertices_);
            }
        }
    }

    else if (draw_mode == "Texture Layout")
    {
        if (n_faces() && has_texcoords_)
        {
            phong_shader_.set_uniform("show_texture_layout", true);
            phong_shader_.set_uniform("use_vertex_color", false);
            phong_shader_.set_uniform("use_lighting", false);

            // draw faces
            phong_shader_.set_uniform("front_color", vec3(0.8, 0.8, 0.8));
            phong_shader_.set_uniform("back_color", vec3(0.9, 0.0, 0.0));
            glDepthRange(0.01, 1.0);
            glDrawArrays(GL_TRIANGLES, 0, n_vertices_);

            // overlay edges
            glDepthRange(0.0, 1.0);
            glDepthFunc(GL_LEQUAL);
            phong_shader_.set_uniform("front_color", vec3(0.1, 0.1, 0.1));
            phong_shader_.set_uniform("back_color", vec3(0.1, 0.1, 0.1));
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, edge_buffer_);
            glDrawElements(GL_LINES, n_edges_, GL_UNSIGNED_INT, nullptr);
            glDepthFunc(GL_LESS);
        }
    }

    // draw feature edges
    if (n_features_)
    {
        phong_shader_.set_uniform("front_color", vec3(0, 1, 0));
        phong_shader_.set_uniform("back_color", vec3(0, 1, 0));
        phong_shader_.set_uniform("use_vertex_color", false);
        phong_shader_.set_uniform("use_lighting", false);
        glDepthRange(0.0, 1.0);
        glDepthFunc(GL_LEQUAL);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, feature_buffer_);
        glDrawElements(GL_LINES, n_features_, GL_UNSIGNED_INT, nullptr);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glDepthFunc(GL_LESS);
    }

    // disable transparency (doesn't work well with imgui)
    glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);

    glBindVertexArray(0);
    glCheckError();
}

void SurfaceMeshGL::tesselate(const std::vector<vec3>& points,
                              std::vector<ivec3>& triangles)
{
    const int n = points.size();

    triangles.clear();
    triangles.reserve(n - 2);

    // triangle? nothing to do
    if (n == 3)
    {
        triangles.push_back(ivec3(0, 1, 2));
        return;
    }

    // quad? simply compare to two options
    else if (n == 4)
    {
        if (area(points[0], points[1], points[2]) +
                area(points[0], points[2], points[3]) <
            area(points[0], points[1], points[3]) +
                area(points[1], points[2], points[3]))
        {
            triangles.push_back(ivec3(0, 1, 2));
            triangles.push_back(ivec3(0, 2, 3));
        }
        else
        {
            triangles.push_back(ivec3(0, 1, 3));
            triangles.push_back(ivec3(1, 2, 3));
        }
        return;
    }

    // n-gon with n>4? compute triangulation by dynamic programming
    init_triangulation(n);
    int i, j, m, k, imin;
    Scalar w, wmin;

    // initialize 2-gons
    for (i = 0; i < n - 1; ++i)
    {
        triangulation(i, i + 1) = Triangulation(0.0, -1);
    }

    // n-gons with n>2
    for (j = 2; j < n; ++j)
    {
        // for all n-gons [i,i+j]
        for (i = 0; i < n - j; ++i)
        {
            k = i + j;

            wmin = std::numeric_limits<Scalar>::max();
            imin = -1;

            // find best split i < m < i+j
            for (m = i + 1; m < k; ++m)
            {
                w = triangulation(i, m).area +
                    area(points[i], points[m], points[k]) +
                    triangulation(m, k).area;

                if (w < wmin)
                {
                    wmin = w;
                    imin = m;
                }
            }

            triangulation(i, k) = Triangulation(wmin, imin);
        }
    }

    // build triangles from triangulation table
    std::vector<ivec2> todo;
    todo.reserve(n);
    todo.push_back(ivec2(0, n - 1));
    while (!todo.empty())
    {
        ivec2 tri = todo.back();
        todo.pop_back();
        int start = tri[0];
        int end = tri[1];
        if (end - start < 2)
            continue;
        int split = triangulation(start, end).split;

        triangles.push_back(ivec3(start, split, end));

        todo.push_back(ivec2(start, split));
        todo.push_back(ivec2(split, end));
    }
}

} // namespace pmp
