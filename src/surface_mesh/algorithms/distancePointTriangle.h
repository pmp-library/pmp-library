//=============================================================================
#pragma once
//=============================================================================

#include <surface_mesh/types.h>

//=============================================================================

namespace surface_mesh {

//=============================================================================

//! distance from point P to line segment (V0,V1)
Scalar distPointLineSegment(const Point& P, const Point& V0, const Point& V1,
                            Point& NearestPoint);

//! distance from point P to triangle (V0, V1, V2)
Scalar distPointTriangle(const Point& P, const Point& V0, const Point& V1,
                         const Point& V2, Point& NearestPoint);

//=============================================================================
} // namespace surface_mesh
//=============================================================================
