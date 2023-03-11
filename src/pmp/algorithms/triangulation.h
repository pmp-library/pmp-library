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

//! \brief Triangulate polygons to get a pure triangle mesh.
//! \details Triangulate n-gons into n-2 triangles. Finds the triangulation that
//! minimizes the sum of squared triangle areas, or the one that maximizes the
//! minimum angle.
//! See \cite liepa_2003_filling for details.
//! \warning Objective::MAX_ANGLE can lead to fold-overs in case of non-convex polygons. Use Objective::MIN_AREA` instead in this case.
//! \ingroup algorithms
void triangulate(SurfaceMesh& mesh,
                 TriangulationObjective o = TriangulationObjective::min_area);

//! Triangulate the Face \p f
//! \pre The input face is manifold
//! \throw InvalidInputException in case the input precondition is violated
//! \warning Objective::MAX_ANGLE can lead to fold-overs in case of non-convex polygons. Use Objective::MIN_AREA` instead in this case.
void triangulate(SurfaceMesh& mesh, Face f,
                 TriangulationObjective o = TriangulationObjective::min_area);

} // namespace pmp
