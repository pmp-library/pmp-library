// Copyright 2011-2022 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include "pmp/surface_mesh.h"

namespace pmp {

enum class BoundaryHandling
{
    Interpolate,
    Preserve
};

//! \brief Perform one step of Catmull-Clark subdivision.
//! \details See \cite catmull_1978_recursively for details.
//! \param mesh The input mesh, modified in place.
//! \param boundary_handling Specify to interpolate or preserve boundary edges.
//! \ingroup algorithms
void catmull_clark_subdivision(
    SurfaceMesh& mesh,
    BoundaryHandling boundary_handling = BoundaryHandling::Interpolate);

//! \brief Perform one step of Loop subdivision.
//! \details See \cite loop_1987_smooth for details.
//! \param mesh The input mesh, modified in place.
//! \param boundary_handling Specify to interpolate or preserve boundary edges.
//! \pre Requires a triangle mesh as input.
//! \throw InvalidInputException in case the input violates the precondition.
//! \ingroup algorithms
void loop_subdivision(SurfaceMesh& mesh, BoundaryHandling boundary_handling =
                                             BoundaryHandling::Interpolate);

//! \brief Perform one step of quad-tri subdivision.
//! \details Suitable for mixed quad/triangle meshes. See \cite stam_2003_subdiv for details.
//! \param mesh The input mesh, modified in place.
//! \param boundary_handling Specify to interpolate or preserve boundary edges.
//! \ingroup algorithms
void quad_tri_subdivision(
    SurfaceMesh& mesh,
    BoundaryHandling boundary_handling = BoundaryHandling::Interpolate);

//! \brief Perform one step of linear quad-tri subdivision.
//! \details Suitable for mixed quad/triangle meshes.
//! \param mesh The input mesh, modified in place.
//! \ingroup algorithms
void linear_subdivision(SurfaceMesh& mesh);

} // namespace pmp
