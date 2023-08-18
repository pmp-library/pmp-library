// Copyright 2011-2022 the Polygon Mesh Processing Library developers.
// Copyright 2020 Astrid Bunge, Philipp Herholz, Misha Kazhdan, Mario Botsch.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include "pmp/surface_mesh.h"
#include "pmp/algorithms/numerics.h"

namespace pmp {

/// compute uniform mass matrix, containing vertex valence on the diagonal
void setup_uniform_mass_matrix(const SurfaceMesh& mesh, DiagonalMatrix& M);

/// compute uniform Laplace matrix
void setup_uniform_laplace_matrix(const SurfaceMesh& mesh, SparseMatrix& M);

/// compute (lumped) mass matrix, containing per-vertex Voronoi areas on the diagonal
void setup_mass_matrix(const SurfaceMesh& mesh, DiagonalMatrix& M);

/// compute cotan Laplace matrix, optionally clamping negative cotan weights to zero
void setup_laplace_matrix(const SurfaceMesh& mesh, SparseMatrix& L,
                          bool clamp = false);

/// compute gradient matrix
void setup_gradient_matrix(const SurfaceMesh& mesh, SparseMatrix& G);

/// compute divergence matrix
void setup_divergence_matrix(const SurfaceMesh& mesh, SparseMatrix& D);

/// compute matrix that has vertex coordinates as rows
void coordinates_to_matrix(const SurfaceMesh& mesh, DenseMatrix& X);

/// set vertex coordinates from rows of matrix X
void matrix_to_coordinates(const DenseMatrix& X, SurfaceMesh& mesh);

} // namespace pmp