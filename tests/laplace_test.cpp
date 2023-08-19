// Copyright 2017-2021 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "gtest/gtest.h"
#include "pmp/algorithms/laplace.h"
#include "pmp/algorithms/shapes.h"
#include "pmp/algorithms/triangulation.h"
#include "pmp/algorithms/differential_geometry.h"

using namespace pmp;

namespace {

// check dimension of Laplace and gradient matrix
// assume a closed mesh without boundary
unsigned int matrix_dimension_error(const SurfaceMesh& mesh)
{
    SparseMatrix L, G, D;
    DiagonalMatrix M;
    laplace_matrix(mesh, L);
    gradient_matrix(mesh, G);
    divergence_matrix(mesh, D);
    mass_matrix(mesh, M);

    unsigned int nv = mesh.n_vertices();
    unsigned int nh = mesh.n_halfedges();

    unsigned int error = 0;
    error += std::abs(L.rows() - nv);
    error += std::abs(L.cols() - nv);
    error += std::abs(G.cols() - nv);
    error += std::abs(G.rows() - 3 * nh);
    error += std::abs(D.cols() - 3 * nh);
    error += std::abs(D.rows() - nv);
    error += std::abs(M.rows() - nv);
    error += std::abs(M.cols() - nv);

    return error;
}

// Laplace should be divergence of gradient
double div_grad_error(const SurfaceMesh& mesh)
{
    SparseMatrix L, G, D;
    laplace_matrix(mesh, L);
    gradient_matrix(mesh, G);
    divergence_matrix(mesh, D);
    return (L - D * G).norm();
}

// Gradient of linear function on PLANAR mesh should be constant
// Test this on gradient of coordinate function
double constant_gradient_error(const SurfaceMesh& mesh)
{
    SparseMatrix G;
    gradient_matrix(mesh, G);
    DenseMatrix X;
    coordinates_to_matrix(mesh, X);
    DenseMatrix GX = G * X;

    DenseMatrix GG = DenseMatrix::Zero(GX.rows(), GX.cols());
    for (int i = 0; i < GG.rows(); i += 3)
    {
        GG(i + 0, 0) = 1.0;
        GG(i + 1, 1) = 1.0;
    }

    return (GX - GG).norm();
}

// Laplace of linear function on PLANAR Mesh should vanish
double linear_precision_error(const SurfaceMesh& mesh)
{
    SparseMatrix L;
    laplace_matrix(mesh, L);
    DenseMatrix X;
    coordinates_to_matrix(mesh, X);
    DenseMatrix LX = L * X;

    double error(0);
    for (auto v : mesh.vertices())
    {
        if (!mesh.is_boundary(v))
        {
            error += LX.row(v.idx()).norm();
        }
    }

    return error;
}

// sum of mass matrix entries should be surface area
double mass_matrix_error(const SurfaceMesh& mesh)
{
    double a1 = surface_area(mesh);

    DiagonalMatrix M;
    mass_matrix(mesh, M);
    double a2 = M.diagonal().sum();

    return std::fabs(a1 - a2);
}

} // anonymous namespace

TEST(LaplaceTest, matrix_dimensions_on_triangles)
{
    auto mesh = icosphere();
    EXPECT_EQ(matrix_dimension_error(mesh), 0u);
}

TEST(LaplaceTest, matrix_dimensions_on_polygons)
{
    auto mesh = uv_sphere();
    EXPECT_EQ(matrix_dimension_error(mesh), 0u);
}

TEST(LaplaceTest, divgrad_on_triangles)
{
    auto mesh = icosphere();
    EXPECT_LT(div_grad_error(mesh), 1e-13);
}

TEST(LaplaceTest, divgrad_on_polygons)
{
    auto mesh = quad_sphere();
    EXPECT_LT(div_grad_error(mesh), 1e-13);
}

TEST(LaplaceTest, constant_gradients_on_quads)
{
    auto mesh = plane();
    EXPECT_LT(constant_gradient_error(mesh), 1e-14);
}

TEST(LaplaceTest, linear_precision_on_triangles)
{
    auto mesh = plane();
    triangulate(mesh);
    EXPECT_LT(linear_precision_error(mesh), 1e-14);
}

TEST(LaplaceTest, linear_precision_on_quads)
{
    auto mesh = plane();
    EXPECT_LT(linear_precision_error(mesh), 1e-14);
}

TEST(LaplaceTest, mass_matrix_on_triangles)
{
    auto mesh = icosphere();
    EXPECT_LT(mass_matrix_error(mesh), 1e-3);
}

TEST(LaplaceTest, mass_matrix_on_quads)
{
    auto mesh = quad_sphere();
    EXPECT_LT(mass_matrix_error(mesh), 1e-3);
}