//=============================================================================
// Copyright (C) 2011-2019 The pmp-library developers
//
// This file is part of the Polygon Mesh Processing Library.
// Distributed under the terms of the MIT license, see LICENSE.txt for details.
//
// SPDX-License-Identifier: MIT
//=============================================================================
#pragma once
//=============================================================================

#include <pmp/types.h>

//=============================================================================

namespace pmp {

//=============================================================================

//! \addtogroup algorithms algorithms
//! @{

//! Compute the distance of a point p to a line segment given by points (v0,v1).
Scalar dist_point_line_segment(const Point& p, const Point& v0, const Point& v1,
                               Point& nearest_point);

//! Compute the distance of a point p to the triangle given by points (v0, v1, v2).
Scalar dist_point_triangle(const Point& p, const Point& v0, const Point& v1,
                           const Point& v2, Point& nearest_point);

//=============================================================================
//! @}
//=============================================================================
} // namespace pmp
//=============================================================================
