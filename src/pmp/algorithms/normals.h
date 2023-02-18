// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include "pmp/SurfaceMesh.h"

namespace pmp {

//! \brief Compute vertex normals for the whole \p mesh.
//! \details Calls vertex_normal() for each vertex and adds a new
//! vertex property of type Normal named "v:normal".
void vertex_normals(SurfaceMesh& mesh);

//! \brief Compute face normals for the whole \p mesh.
//! \details Calls face_normal() for each face and adds a new face
//! property of type Normal named "f:normal".
void face_normals(SurfaceMesh& mesh);

//! \brief Compute the normal vector of vertex \p v.
Normal vertex_normal(const SurfaceMesh& mesh, Vertex v);

//! \brief Compute the normal vector of face \p f.
//! \details Normal is computed as (normalized) sum of per-corner
//! cross products of the two incident edges. This corresponds to
//! the normalized vector area in \cite alexa_2011_laplace
Normal face_normal(const SurfaceMesh& mesh, Face f);

//! \brief Compute the normal vector of the polygon corner specified by the
//! target vertex of halfedge \p h.
//! \details Averages incident corner normals if they are within crease_angle
//! of the face normal. \p crease_angle is in radians, not degrees.
Normal corner_normal(const SurfaceMesh& mesh, Halfedge h, Scalar crease_angle);

} // namespace pmp
