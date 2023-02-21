// Copyright 2011-2021 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include <vector>

#include "pmp/SurfaceMesh.h"

namespace pmp {

//! Triangulation objective
enum class TriangulationObjective
{
    min_area, //!< minimize the sum of squared areas
    max_angle //!< maximize the minimum angle
};

//! Triangulate all faces
void triangulate(SurfaceMesh& mesh,
                 TriangulationObjective o = TriangulationObjective::min_area);

//! Triangulate the Face \p f
//! \pre The input face is manifold
//! \throw InvalidInputException in case the input precondition is violated
void triangulate(SurfaceMesh& mesh, Face f,
                 TriangulationObjective o = TriangulationObjective::min_area);

} // namespace pmp
