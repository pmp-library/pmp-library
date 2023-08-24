// Copyright 2011-2021 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include "pmp/surface_mesh.h"

namespace pmp {

//! \brief Triangulate all faces in \p mesh by applying triangulate().
//! \ingroup algorithms
void triangulate(SurfaceMesh& mesh);

//! \brief Triangulate the Face \p f .
//! \details Triangulate n-gons into n-2 triangles. Finds the triangulation that
//! minimizes the sum of squared triangle areas.
//! See \cite liepa_2003_filling for details.
//! \pre The input face is manifold
//! \note This algorithm works on general polygon meshes.
//! \throw InvalidInputException in case the input precondition is violated
//! \ingroup algorithms
void triangulate(SurfaceMesh& mesh, Face f);

} // namespace pmp
