// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include "pmp/surface_mesh.h"

namespace pmp {

//! \brief Mesh decimation based on approximation error and fairness
//! criteria.
//! \details Performs incremental greedy mesh decimation based on halfedge
//! collapses. See \cite kobbelt_1998_general and \cite garland_1997_surface for details.
//! \param mesh Target mesh. Modified in place.
//! \param n_vertices Target number of vertices.
//! \param aspect_ratio Minimum aspect ratio of the triangles.
//! \param edge_length Minimum target edge length.
//! \param max_valence Maximum number of incident edges per vertex.
//! \param normal_deviation Maximum deviation of face normals.
//! \param hausdorff_error Maximum deviation from the original surface.
//! \param seam_threshold Threshold for texture seams.
//! \param seam_angle_deviation Maximum texture seam deviation.
//! \pre Input mesh needs to be a triangle mesh.
//! \throw InvalidInputException if the input precondition is violated.
//! \ingroup algorithms
void decimate(SurfaceMesh& mesh, unsigned int n_vertices,
              Scalar aspect_ratio = 0.0, Scalar edge_length = 0.0,
              unsigned int max_valence = 0, Scalar normal_deviation = 0.0,
              Scalar hausdorff_error = 0.0, Scalar seam_threshold = 1e-2,
              Scalar seam_angle_deviation = 1);

} // namespace pmp
