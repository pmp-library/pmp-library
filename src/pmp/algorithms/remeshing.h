// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include "pmp/SurfaceMesh.h"

#include <memory>

namespace pmp {

//! \brief Perform uniform remeshing.
//! \details Performs incremental remeshing based
//! on edge collapse, split, flip, and tangential relaxation.
//! See \cite botsch_2004_remeshing and \cite dunyach_2013_adaptive for details.
//! \param edge_length the target edge length.
//! \param iterations the number of iterations
//! \param use_projection use back-projection to the input surface
//! \pre Input mesh needs to be a pure triangle mesh.
//! \throw InvalidInputException if the input precondition is violated.
void uniform_remeshing(SurfaceMesh& mesh, Scalar edge_length,
                       unsigned int iterations = 10,
                       bool use_projection = true);

//! \brief Perform adaptive remeshing.
//! \details Performs incremental remeshing based
//! on edge collapse, split, flip, and tangential relaxation.
//! See \cite botsch_2004_remeshing and \cite dunyach_2013_adaptive for details.
//! \param min_edge_length the minimum edge length.
//! \param max_edge_length the maximum edge length.
//! \param approx_error the maximum approximation error
//! \param iterations the number of iterations
//! \param use_projection use back-projection to the input surface
//! \pre Input mesh needs to be a pure triangle mesh.
//! \throw InvalidInputException if the input precondition is violated.
void adaptive_remeshing(SurfaceMesh& mesh, Scalar min_edge_length,
                        Scalar max_edge_length, Scalar approx_error,
                        unsigned int iterations = 10,
                        bool use_projection = true);

} // namespace pmp
