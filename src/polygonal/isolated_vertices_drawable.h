// Copyright 2026 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#pragma once

#include <vector>

#include "pmp/surface_mesh.h"
#include "pmp/viewers/drawable.h"
#include "pmp/viewers/shader.h"

namespace pmp {

class IsolatedVerticesDrawable : public Drawable
{
public:
    explicit IsolatedVerticesDrawable(const SurfaceMesh& mesh);
    ~IsolatedVerticesDrawable() override;

    void update_buffers() override;
    void draw(const mat4& projection, const mat4& modelview) override;

private:
    const SurfaceMesh& mesh_;
    GLuint vertex_array_object_;
    GLuint vertex_buffer_;
    std::vector<vec3> positions_;
    Shader shader_;
};

} // namespace pmp
