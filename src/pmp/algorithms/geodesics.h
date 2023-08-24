// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include <limits>
#include <vector>

#include "pmp/surface_mesh.h"

namespace pmp {

//! \brief Compute geodesic distance from a set of seed vertices
//! \details The method works by a Dijkstra-like breadth first traversal from
//! the seed vertices, implemented by a heap structure.
//! See \cite kimmel_1998_geodesic for details.
//! \param mesh The input mesh, modified in place.
//! \param[in] seeds The vector of seed vertices.
//! \param[in] maxdist The maximum distance up to which to compute the
//! geodesic distances.
//! \param[in] maxnum The maximum number of neighbors up to which to
//! compute the geodesic distances.
//! \param[out] neighbors The vector of neighbor vertices.
//! \return The number of neighbors that have been found.
//! \pre Input mesh needs to be a triangle mesh.
//! \ingroup algorithms
unsigned int geodesics(
    SurfaceMesh& mesh, const std::vector<Vertex>& seeds,
    Scalar maxdist = std::numeric_limits<Scalar>::max(),
    unsigned int maxnum = std::numeric_limits<unsigned int>::max(),
    std::vector<Vertex>* neighbors = nullptr);

//! \brief Compute geodesic distance from a set of seed vertices
//! \details Compute geodesic distances based on the heat method,
//! by solving two Poisson systems. Works on general polygon meshes.
//! See \cite crane_2013_geodesics for details.
//! \param mesh The input mesh, modified in place.
//! \param seeds The vector of seed vertices.
//! \note This algorithm works on general polygon meshes.
//! \ingroup algorithms
void geodesics_heat(SurfaceMesh& mesh, const std::vector<Vertex>& seeds);

//! \brief Use the normalized distances as texture coordinates
//! \details Stores the normalized distances in a vertex property of type
//! TexCoord named "v:tex". Re-uses any existing vertex property of the
//! same type and name.
void distance_to_texture_coordinates(SurfaceMesh& mesh);

} // namespace pmp
