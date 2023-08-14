// Copyright 2011-2022 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include "pmp/surface_mesh.h"

namespace pmp {

//! \brief Perform one step of Catmull-Clark subdivision.
//! \details See \cite catmull_1978_recursively for details.
//! \ingroup algorithms
void catmull_clark_subdivision(SurfaceMesh& mesh);

//! \brief Perform one step of Loop subdivision.
//! \details See \cite loop_1987_smooth for details.
//! \pre Requires a triangle mesh as input.
//! \throw InvalidInputException in case the input violates the precondition.
//! \ingroup algorithms
void loop_subdivision(SurfaceMesh& mesh);

//! \brief Perform one step of quad-tri subdivision.
//! \details Suitable for mixed quad/triangle meshes. See \cite stam_2003_subdiv for details.
//! \ingroup algorithms
void quad_tri_subdivision(SurfaceMesh& mesh);

} // namespace pmp
