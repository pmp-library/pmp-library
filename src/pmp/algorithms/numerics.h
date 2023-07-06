// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include "pmp/SurfaceMesh.h"
#include <Eigen/Sparse>
#include <Eigen/Dense>

namespace pmp {

using SparseMatrix = Eigen::SparseMatrix<double>;
using DiagonalMatrix = Eigen::DiagonalMatrix<double, Eigen::Dynamic>;
using DenseMatrix = Eigen::MatrixXd;
using Triplet = Eigen::Triplet<double>;

DenseMatrix cholesky_solve(const SparseMatrix& A, const DenseMatrix& B);

DenseMatrix cholesky_solve(const SparseMatrix& A, const DenseMatrix& B,
                           std::function<bool(unsigned int)> is_constrained,
                           const DenseMatrix& C);

void setup_selector_matrix(const SurfaceMesh& mesh,
                           std::function<bool(Vertex)> is_selected,
                           SparseMatrix& S);

} // namespace pmp