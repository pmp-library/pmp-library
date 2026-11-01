// Copyright 2025 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#pragma once

#include <pmp/types.h>

namespace pmp {

//! Orientation of points in 2D and 3D.
enum class Orientation
{
    Negative = -1,
    Coplanar = 0,
    Positive = 1
};

//! 3D orientation test using exact predicates.
Orientation orient3d(const dvec3& a, const dvec3& b, const dvec3& c,
                     const dvec3& d);

} // namespace pmp
