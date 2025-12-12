// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include "pmp/surface_mesh.h"

namespace pmp {

//! \brief Perform uniform remeshing.
//! \details Performs incremental remeshing based
//! on edge collapse, split, flip, and tangential relaxation.
//! See \cite botsch_2004_remeshing and \cite dunyach_2013_adaptive for details.
//! \param mesh The input mesh, modified in place.
//! \param edge_length The target edge length.
//! \param iterations The number of iterations
//! \param use_projection Use back-projection to the input surface.
//! \pre Input mesh needs to be a triangle mesh.
//! \throw InvalidInputException if the input precondition is violated.
//! \ingroup algorithms
void uniform_remeshing(SurfaceMesh& mesh, Scalar edge_length,
                       unsigned int iterations = 10,
                       bool use_projection = true);

//! \brief Perform adaptive remeshing.
//! \details Performs incremental remeshing based
//! on edge collapse, split, flip, and tangential relaxation.
//! See \cite botsch_2004_remeshing and \cite dunyach_2013_adaptive for details.
//! \param mesh The input mesh, modified in place.
//! \param min_edge_length The minimum edge length.
//! \param max_edge_length The maximum edge length.
//! \param approx_error The maximum approximation error.
//! \param iterations The number of iterations.
//! \param use_projection Use back-projection to the input surface.
//! \pre Input mesh needs to be a triangle mesh.
//! \throw InvalidInputException if the input precondition is violated.
//! \ingroup algorithms
void adaptive_remeshing(SurfaceMesh& mesh, Scalar min_edge_length,
                        Scalar max_edge_length, Scalar approx_error,
                        unsigned int iterations = 10,
                        bool use_projection = true);


void custom_remeshing(SurfaceMesh& mesh, std::vector<double>& target_edge_lengths,
                        unsigned int iterations = 10,
                        bool use_projection = true);

} // namespace pmp
