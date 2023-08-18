// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include "pmp/surface_mesh.h"
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

//! Build SurfaceMesh from Eigen matrices containing vertex coordinates and triangle indices.
//! \param V \f$n\times 3\f$ matrix of double precision vertex coordinates.
//! \param F \f$m\times 3\f$ matrix of integer triangle indices.
//! \param mesh The mesh to be build from \p V and \p F . The mesh will be cleared.
void matrices_to_mesh(const Eigen::MatrixXd& V, const Eigen::MatrixXi& F,
                      pmp::SurfaceMesh& mesh);

//! Convert SurfaceMesh to Eigen matrices of vertex coordinates and triangle indices.
//! \param mesh The mesh used to fill \p V and \p F .
//! \param V The resulting \f$n\times 3\f$ matrix of double precision vertex coordinates.
//! \param F The resulting \f$m\times 3\f$ matrix of integer triangle indices.
void mesh_to_matrices(const pmp::SurfaceMesh& mesh, Eigen::MatrixXd& V,
                      Eigen::MatrixXi& F);

} // namespace pmp