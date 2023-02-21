// Copyright 2011-2021 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include <vector>

#include "pmp/SurfaceMesh.h"

namespace pmp {

//! Triangulation objective
enum class TriangulationObjective
{
    MIN_AREA, //!< minimize the sum of squared areas
    MAX_ANGLE //!< maximize the minimum angle
};

//! Triangulate all faces
void triangulate(SurfaceMesh& mesh,
                 TriangulationObjective o = TriangulationObjective::MIN_AREA);

//! Triangulate the Face \p f
//! \pre The input face is manifold
//! \throw InvalidInputException in case the input precondition is violated
void triangulate(SurfaceMesh& mesh, Face f,
                 TriangulationObjective o = TriangulationObjective::MIN_AREA);

} // namespace pmp
