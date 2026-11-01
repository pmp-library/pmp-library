// Copyright 2026 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#include "isolated_vertices_drawable.h"

#include "pmp/viewers/gl.h"

namespace pmp {
namespace {

const char* vertex_shader =
#ifndef __EMSCRIPTEN__
    "#version 330\n"
#else
    "#version 300 es\n"
#endif
    R"glsl(
layout(location = 0) in vec3 position;
uniform mat4 modelview_projection_matrix;
uniform float point_size;

void main()
{
    gl_Position = modelview_projection_matrix * vec4(position, 1.0);
    gl_PointSize = point_size;
}
)glsl";

const char* fragment_shader =
#ifndef __EMSCRIPTEN__
    "#version 330\n"
#else
    "#version 300 es\n"
#endif
    R"glsl(
precision mediump float;
uniform vec3 color;
out vec4 frag_color;

void main()
{
    vec2 uv = 2.0 * gl_PointCoord - 1.0;
    if (dot(uv, uv) > 1.0)
        discard;
    frag_color = vec4(color, 1.0);
}
)glsl";

} // namespace

IsolatedVerticesDrawable::IsolatedVerticesDrawable(const SurfaceMesh& mesh)
    : mesh_(mesh), vertex_array_object_(0), vertex_buffer_(0)
{
    glGenVertexArrays(1, &vertex_array_object_);
    glGenBuffers(1, &vertex_buffer_);
    shader_.source(vertex_shader, fragment_shader);
}

IsolatedVerticesDrawable::~IsolatedVerticesDrawable()
{
    glDeleteBuffers(1, &vertex_buffer_);
    glDeleteVertexArrays(1, &vertex_array_object_);
}

void IsolatedVerticesDrawable::update_buffers()
{
    positions_.clear();
    const auto selected = mesh_.get_vertex_property<bool>("v:selected");
    const auto position = mesh_.get_vertex_property<Point>("v:point");
    if (mesh_.n_faces() && selected)
    {
        for (auto v : mesh_.vertices())
        {
            if (mesh_.is_isolated(v) && selected[v])
                positions_.push_back((vec3)position[v]);
        }
    }

    glBindVertexArray(vertex_array_object_);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
    glBufferData(GL_ARRAY_BUFFER, positions_.size() * sizeof(vec3),
                 positions_.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

void IsolatedVerticesDrawable::draw(const mat4& projection,
                                    const mat4& modelview)
{
    if (positions_.empty())
        return;

    shader_.use();
    shader_.set_uniform("modelview_projection_matrix", projection * modelview);
    shader_.set_uniform("point_size", 5.0f);
    shader_.set_uniform("color", vec3(0, 1, 1));

#ifndef __EMSCRIPTEN__
    glEnable(GL_PROGRAM_POINT_SIZE);
#endif
    glDepthFunc(GL_LEQUAL);
    glBindVertexArray(vertex_array_object_);
    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(positions_.size()));
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}

} // namespace pmp
