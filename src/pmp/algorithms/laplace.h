// Copyright 2011-2022 the Polygon Mesh Processing Library developers.
// Copyright 2020 Astrid Bunge, Philipp Herholz, Misha Kazhdan, Mario Botsch.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include "pmp/surface_mesh.h"
#include "pmp/algorithms/numerics.h"

namespace pmp {

/// compute cotan mass matrix
void setup_mass_matrix(const SurfaceMesh& mesh, DiagonalMatrix& M,
                       bool uniform_laplace = false);

/// compute cotan stiffness matrix
void setup_laplace_matrix(const SurfaceMesh& mesh, SparseMatrix& L,
                          bool uniform_laplace = false, bool clamp = false);

/// compute matrix that has vertex coordinates as rows
void coordinates_to_matrix(const SurfaceMesh& mesh, DenseMatrix& X);

/// set vertex coordinates from row of matrix X
void matrix_to_coordinates(const DenseMatrix& X, SurfaceMesh& mesh);

} // namespace pmp