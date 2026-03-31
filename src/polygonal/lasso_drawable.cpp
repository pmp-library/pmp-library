// Copyright 2026 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#include "lasso_drawable.h"
#include "lasso_shader.h"
#include "pmp/mat_vec.h"
#include "pmp/viewers/gl.h"

namespace pmp {

LassoDrawable::LassoDrawable() : lasso_vao_(0), lasso_vbo_(0), lasso_buffer_(0)
{
    // Generate OpenGL buffers
    glGenVertexArrays(1, &lasso_vao_);
    glGenBuffers(1, &lasso_vbo_);
    glGenBuffers(1, &lasso_buffer_);

    lasso_shader_.source(lasso_vshader, lasso_fshader);
}

LassoDrawable::~LassoDrawable()
{
    // Delete OpenGL buffers
    glDeleteBuffers(1, &lasso_vbo_);
    glDeleteBuffers(1, &lasso_buffer_);
    glDeleteVertexArrays(1, &lasso_vao_);
}

void LassoDrawable::update_lasso(const std::vector<ivec2>& lasso_points_screen)
{
    lasso_points_ndc_.clear();

    // get viewport data
    std::array<GLint, 4> viewport;
    glGetIntegerv(GL_VIEWPORT, viewport.data());

    // screen (x,y) to ndc
    for (const auto& p : lasso_points_screen)
    {
        float x_ndc = (2.0f * (p[0] - viewport[0])) / viewport[2] - 1.0f;
        float y_ndc =
            (2.0f * (viewport[3] - (p[1] - viewport[1]))) / viewport[3] - 1.0f;
        lasso_points_ndc_.emplace_back(x_ndc, y_ndc);
    }

    // Close the lasso by adding the first point at the end
    if (!lasso_points_ndc_.empty())
    {
        lasso_points_ndc_.push_back(lasso_points_ndc_.front());
    }

    update_buffers();
}

void LassoDrawable::update_buffers()
{
    if (lasso_points_ndc_.empty())
        return;

    glBindVertexArray(lasso_vao_);
    glBindBuffer(GL_ARRAY_BUFFER, lasso_vbo_);
    glBufferData(GL_ARRAY_BUFFER, lasso_points_ndc_.size() * sizeof(vec2),
                 lasso_points_ndc_.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

void LassoDrawable::draw(const mat4&, const mat4&)
{
    if (lasso_points_ndc_.empty())
        return;

    lasso_shader_.use();
    lasso_shader_.set_uniform("color", vec3(0, 1, 1));
    glBindVertexArray(lasso_vao_);
    glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_LINE_STRIP, 0, lasso_points_ndc_.size());
    glEnable(GL_DEPTH_TEST);
    glBindVertexArray(0);
}

} // namespace pmp
