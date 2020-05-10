// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include "pmp/SurfaceMesh.h"

namespace pmp {

//! \brief A class for computing surface normals.
//! \details This class provides a set of static functions for computing surface
//! normal information, either
//!
//! \li per vertex: compute_vertex_normal()
//! \li per face: compute_face_normal()
//! \li per corner: compute_corner_normal()
//!
//! The convenience functions compute_vertex_normals() and compute_face_normals()
//! compute the normals for the whole mesh and add a corresponding vertex or
//! face property.
//! \ingroup algorithms
class SurfaceNormals
{
public:
    // delete default and copy constructor
    SurfaceNormals() = delete;
    SurfaceNormals(const SurfaceNormals&) = delete;

    //! \brief Compute vertex normals for the whole \p mesh.
    //! \details Calls compute_vertex_normal() for each vertex and adds a new
    //! vertex property of type Normal named "v:normal".
    static void compute_vertex_normals(SurfaceMesh& mesh);

    //! \brief Compute face normals for the whole \p mesh.
    //! \details Calls compute_face_normal() for each face and adds a new face
    //! property of type Normal named "f:normal".
    static void compute_face_normals(SurfaceMesh& mesh);

    //! \brief Compute the normal vector of vertex \p v.
    static Normal compute_vertex_normal(const SurfaceMesh& mesh, Vertex v);

    //! \brief Compute the normal vector of face \p f.
    //! \details Normal is computed as (normalized) sum of per-corner
    //! cross products of the two incident edges. This corresponds to
    //! the normalized vector area in \cite alexa_2011_laplace
    static Normal compute_face_normal(const SurfaceMesh& mesh, Face f);

    //! \brief Compute the normal vector of the polygon corner specified by the
    //! target vertex of halfedge \p h.
    //! \details Averages incident corner normals if they are within crease_angle
    //! of the face normal. \p crease_angle is in radians, not degrees.
    static Normal compute_corner_normal(const SurfaceMesh& mesh, Halfedge h,
                                        Scalar crease_angle);
};

} // namespace pmp
