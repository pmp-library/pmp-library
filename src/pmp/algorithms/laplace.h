// Copyright 2011-2023 the Polygon Mesh Processing Library developers.
// Copyright 2020 Astrid Bunge, Philipp Herholz, Misha Kazhdan, Mario Botsch.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include "pmp/surface_mesh.h"
#include "pmp/algorithms/numerics.h"

namespace pmp {

//! \brief Construct the mass matrix for the uniform Laplacian.
//! \details Matrix is diagonal and positive definite.
//! M(i,i) is the valence of vertex i.
//! \param mesh The input mesh.
//! \param M The output matrix.
//! \ingroup algorithms
void uniform_mass_matrix(const SurfaceMesh& mesh, DiagonalMatrix& M);

//! \brief Construct the uniform Laplace matrix.
//! \details Matrix is sparse, symmetric and negative semi-definite.
//! M(i,i) is the negative valence of vertex i. M(i,j) is +1 if vertex i and vertex j are neighbors.
//! \param mesh The input mesh.
//! \param L The output matrix.
//! \ingroup algorithms
void uniform_laplace_matrix(const SurfaceMesh& mesh, SparseMatrix& L);

//! \brief Construct the (lumped) mass matrix for the cotangent Laplacian.
//! \details Matrix is diagonal and positive definite.
//! M(i,i) is the (mixed) Voronoi area of vertex i.
//! See \cite meyer_2003_discrete for details on triangle meshes and \cite bunge_2020_polygon for details on polygon meshes.
//! \param mesh The input mesh.
//! \param M The output matrix.
//! \ingroup algorithms
void mass_matrix(const SurfaceMesh& mesh, DiagonalMatrix& M);

//! \brief Construct the cotan Laplace matrix.
//! \details Matrix is sparse, symmetric and negative semi-definite.
//! M(i,i) is the negative valence of vertex i. M(i,j) is cotangent weight of edge (i,j). M(i,i) is negative sum of off-diagonals.
//! The discrete operators are consistent, such that Laplacian is divergence of gradient.
//! See \cite meyer_2003_discrete for details on triangle meshes and \cite bunge_2020_polygon for details on polygon meshes.
//! \param mesh The input mesh.
//! \param L The output matrix.
//! \param clamp Whether or not negative off-diagonal entries should be clamped to zero.
//! \sa gradient_matrix
//! \sa divergence_matrix
//! \ingroup algorithms
void laplace_matrix(const SurfaceMesh& mesh, SparseMatrix& L,
                    bool clamp = false);

//! \brief Construct the cotan gradient matrix.
//! \details Matrix is sparse and maps values at vertices to constant gradient 3D-vectors at non-boundary halfedges.
//! The discrete operators are consistent, such that Laplacian is divergence of gradient.
//! See \cite meyer_2003_discrete for details on triangle meshes and \cite bunge_2020_polygon for details on polygon meshes.
//! \param mesh The input mesh.
//! \param G The output matrix.
//! \sa laplace_matrix
//! \sa divergence_matrix
//! \ingroup algorithms
void gradient_matrix(const SurfaceMesh& mesh, SparseMatrix& G);

//! \brief Construct the cotan divergence matrix.
//! \details Matrix is sparse and maps constant gradient vectors at non-boundary halfedges to values at vertices.
//! The discrete operators are consistent, such that Laplacian is divergence of gradient.
//! See \cite meyer_2003_discrete for details on triangle meshes and \cite bunge_2020_polygon for details on polygon meshes.
//! \param mesh The input mesh.
//! \param D The output matrix.
//! \sa laplace_matrix
//! \sa gradient_matrix
//! \ingroup algorithms
void divergence_matrix(const SurfaceMesh& mesh, SparseMatrix& D);

//! For a mesh with N vertices, construct an Nx3 matrix containing the vertex coordinates in its rows.
//! \param mesh The input mesh.
//! \param X The output matrix.
//! \ingroup algorithms
void coordinates_to_matrix(const SurfaceMesh& mesh, DenseMatrix& X);

//! For a mesh with N vertices, set the vertex coordinates from the rows of an Nx3 matrix.
//! \param X The input matrix.
//! \param mesh The mesh.
//! \ingroup algorithms
void matrix_to_coordinates(const DenseMatrix& X, SurfaceMesh& mesh);

} // namespace pmp