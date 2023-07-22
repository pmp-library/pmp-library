// Copyright 2011-2021 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include "pmp/surface_mesh.h"

namespace pmp {

//! \brief Triangulate polygons to get a triangle mesh.
//! \details Triangulate n-gons into n-2 triangles. Finds the triangulation that
//! minimizes the sum of squared triangle areas.
//! See \cite liepa_2003_filling for details.
//! \ingroup algorithms
void triangulate(SurfaceMesh& mesh);

//! \brief Triangulate the Face \p f
//! \pre The input face is manifold
//! \throw InvalidInputException in case the input precondition is violated
//! \ingroup algorithms
void triangulate(SurfaceMesh& mesh, Face f);

} // namespace pmp
