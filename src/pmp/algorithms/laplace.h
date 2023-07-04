// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include "pmp/SurfaceMesh.h"
#include "pmp/algorithms/numerics.h"

namespace pmp {

/// compute cotan mass matrix
void setup_mass_matrix(const SurfaceMesh& mesh, DiagonalMatrix& M,
                       bool uniform_laplace = false);
// void setup_mass_matrix(const SurfaceMesh& mesh, SparseMatrix& M,
//                        bool uniform_laplace = false);

/// compute cotan stiffness matrix
void setup_stiffness_matrix(const SurfaceMesh& mesh, SparseMatrix& S,
                            bool uniform_laplace = false, bool clamp = false);

} // namespace pmp