// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include "pmp/surface_mesh.h"

namespace pmp {

//! \brief Fill the hole specified by halfedge \p h
//! \details Close simple holes (boundary loops of manifold vertices) by first
//! filling the hole with an angle/area-minimizing triangulation, followed
//! by isometric remeshing, and finished by curvature-minimizing fairing of the
//! filled-in patch.
//! See \cite liepa_2003_filling for details.
//! \pre The specified halfedge is valid.
//! \pre The specified halfedge is a boundary halfedge.
//! \pre The specified halfedge is not adjacent to a non-manifold hole.
//! \throw InvalidInputException in case on of the input preconditions is violated
//! \note This algorithm works on general polygon meshes.
//! \ingroup algorithms
void fill_hole(SurfaceMesh& mesh, Halfedge h);

} // namespace pmp
