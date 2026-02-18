// Copyright 2026 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#pragma once

#include "pmp/mat_vec.h"

namespace pmp {

//! Abstract base class for drawable objects
//! \ingroup viewers
class Drawable
{
public:
    virtual ~Drawable() = default;

    //! Update OpenGL buffers
    virtual void update_buffers() = 0;

    //! Draw the object
    //! \param projection Projection matrix
    //! \param modelview Modelview matrix
    virtual void draw(const mat4& projection, const mat4& modelview) = 0;
};

} // namespace pmp
