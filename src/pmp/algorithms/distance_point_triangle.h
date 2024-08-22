// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include "pmp/types.h"

namespace pmp {

//! \addtogroup algorithms
//! @{

//! Compute the distance of a point \p p to a line segment given by points \p v0 and \p v1.
Scalar dist_point_line_segment(const Point& p, const Point& v0, const Point& v1,
                               Point& nearest_point);

//! Compute the distance of a point \p p to the triangle given by points \p v0, \p v1, \p v2.
Scalar dist_point_triangle(const Point& p, const Point& v0, const Point& v1,
                           const Point& v2, Point& nearest_point);

//! @}

} // namespace pmp
