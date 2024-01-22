// Copyright 2011-2022 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include "pmp/surface_mesh.h"

namespace pmp {

//! \brief Perform one step of Catmull-Clark subdivision.
//! \details See \cite catmull_1978_recursively for details.
//! \param mesh The input mesh, modified in place.
//! \param preserve_boundary Whether to interpolate boundary edges of the input mesh
//! \ingroup algorithms
void catmull_clark_subdivision(SurfaceMesh& mesh, bool preserve_boundary=false);

//! \brief Perform one step of Loop subdivision.
//! \details See \cite loop_1987_smooth for details.
//! \param mesh The input mesh, modified in place.
//! \param preserve_boundary Whether to interpolate boundary edges of the input mesh
//! \pre Requires a triangle mesh as input.
//! \throw InvalidInputException in case the input violates the precondition.
//! \ingroup algorithms
void loop_subdivision(SurfaceMesh& mesh, bool preserve_boundary=false);

//! \brief Perform one step of quad-tri subdivision.
//! \details Suitable for mixed quad/triangle meshes. See \cite stam_2003_subdiv for details.
//! \param mesh The input mesh, modified in place.
//! \param preserve_boundary Whether to interpolate boundary edges of the input mesh
//! \ingroup algorithms
void quad_tri_subdivision(SurfaceMesh& mesh, bool preserve_boundary=false);

} // namespace pmp
