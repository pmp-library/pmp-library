// Copyright 2011-2023 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include "pmp/surface_mesh.h"
#include <Eigen/Sparse>
#include <Eigen/Dense>

namespace pmp {

//! PMP uses Eigen's double-precision sparse matrices
using SparseMatrix = Eigen::SparseMatrix<double>;
//! PMP uses Eigen's double-precision diagonal matrices
using DiagonalMatrix = Eigen::DiagonalMatrix<double, Eigen::Dynamic>;
//! PMP uses Eigen's double-precision dense matrices
using DenseMatrix = Eigen::MatrixXd;
//! PMP uses Eigen's double-precision triplets
using Triplet = Eigen::Triplet<double>;

//! Solve the linear system A*X=B using sparse Cholesky decomposition.
//! Returns the solution vector/matrix X.
//! \pre The matrix A has to be sparse, symmetric, and positive definite.
//! \param A The system matrix.
//! \param B The right hand side.
DenseMatrix cholesky_solve(const SparseMatrix& A, const DenseMatrix& B);

//! Solve the linear system A*X=B with given hard constraints using sparse Cholesky decomposition.
//! Returns the solution vector or matrix X.
//! \pre The matrix A has to be sparse, symmetric, and positive definite.
//! \param A The system matrix.
//! \param B The right hand side.
//! \param is_constrained A function returning whether or not X(i) is constrained or not.
//! \param C A matrix storing the Dirichlet constraints: X(i) should be C(i) is entry i is constrained.
DenseMatrix cholesky_solve(
    const SparseMatrix& A, const DenseMatrix& B,
    const std::function<bool(unsigned int)>& is_constrained,
    const DenseMatrix& C);

//! Constructs a selector matrix for a mesh with N vertices.
//! Returns a matrix built from the rows of the NxN identity matrix that belong to selected vertices.
//! \param mesh The input mesh.
//! \param is_selected A function returning whether or not vertex i is selected or not.
//! \param S The output matrix.
void selector_matrix(const SurfaceMesh& mesh,
                     const std::function<bool(Vertex)>& is_selected,
                     SparseMatrix& S);

//! Build SurfaceMesh from Eigen matrices containing vertex coordinates and triangle indices.
//! \param V \f$n\times 3\f$ matrix of double precision vertex coordinates.
//! \param F \f$m\times 3\f$ matrix of integer triangle indices.
//! \param mesh The mesh to be build from \p V and \p F . The mesh will be cleared.
void matrices_to_mesh(const Eigen::MatrixXd& V, const Eigen::MatrixXi& F,
                      SurfaceMesh& mesh);

//! Convert SurfaceMesh to Eigen matrices of vertex coordinates and triangle indices.
//! \param mesh The mesh used to fill \p V and \p F .
//! \param V The resulting \f$n\times 3\f$ matrix of double precision vertex coordinates.
//! \param F The resulting \f$m\times 3\f$ matrix of integer triangle indices.
void mesh_to_matrices(const SurfaceMesh& mesh, Eigen::MatrixXd& V,
                      Eigen::MatrixXi& F);

} // namespace pmp