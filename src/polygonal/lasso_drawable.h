// Copyright 2026 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#pragma once

#include <vector>
#include "pmp/viewers/drawable.h"
#include "pmp/viewers/shader.h"
#include "pmp/mat_vec.h"

namespace pmp {

// Class for rendering a lasso (selection path) overlay
class LassoDrawable : public Drawable
{
public:
    LassoDrawable();
    ~LassoDrawable();

    void update_lasso(const std::vector<ivec2>& lasso_points_screen);
    void update_buffers() override;
    void draw(const mat4& projection, const mat4& modelview) override;

private:
    // OpenGL buffers
    GLuint lasso_vao_;
    GLuint lasso_vbo_;
    GLuint lasso_buffer_;

    // lasso data
    std::vector<vec2> lasso_points_ndc_;

    // shader
    Shader lasso_shader_;
};

} // namespace pmp
