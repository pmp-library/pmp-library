// Copyright 2017-2021 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#include "gtest/gtest.h"
#include "pmp/algorithms/laplace.h"
#include "pmp/algorithms/shapes.h"
#include "pmp/algorithms/triangulation.h"
#include "pmp/algorithms/differential_geometry.h"

// #include "pmp/algorithms/wild_laplace.h"
#include <Eigen/Eigenvalues>
#include <cstdlib>

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

    // if (mesh.n_vertices() < 100)
    // {
    //     std::cout << "\nL\n" << L << std::endl;
    //     std::cout << "\nD*G\n" << D*G << std::endl;
    //     std::cout << "\nL-D*G\n" << L-D*G << std::endl;
    // }

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

// Laplace matrix should have (only) one vanishing eigenvalue
double eigenvalues_error(const SurfaceMesh& mesh)
{
    // sparse Laplace matrix
    SparseMatrix L;
    laplace_matrix(mesh, L);

    // dense Laplace matrix
    Eigen::MatrixXd A = L;

    // compute eigenvalues, sort in increasing order
    Eigen::VectorXd eigenvalues =
        A.selfadjointView<Eigen::Lower>().eigenvalues().reverse();
    std::cerr << "EIGENVALUES: " << eigenvalues << std::endl;

    return eigenvalues[1];
}

// pick n random edges and co-locate their two vertices
void degenerate_some_edges(SurfaceMesh& mesh, unsigned int n = 10,
                           double h = 1e-5)
{
    const unsigned int n_edges = mesh.n_edges();
    while (n)
    // for (unsigned int i=0; i<n; ++i)
    {
        Edge e(std::rand() % n_edges);
        Vertex v0 = mesh.vertex(e, 0);
        Vertex v1 = mesh.vertex(e, 1);
        if (!mesh.is_boundary(v0) && !mesh.is_boundary(v1))
        {
            Point p0 = mesh.position(v0);
            Point p1 = mesh.position(v1);
            Point m = 0.5 * (p0 + p1);
            Point d = p1 - p0;
            mesh.position(v0) = m - h * d;
            mesh.position(v1) = m + h * d;
            --n;
        }
    }
}

} // anonymous namespace

TEST(LaplaceTest, matrix_dimensions)
{
    auto tri_mesh = icosphere();
    EXPECT_EQ(matrix_dimension_error(tri_mesh), 0u);

    auto quad_mesh = uv_sphere();
    EXPECT_EQ(matrix_dimension_error(quad_mesh), 0u);
}

TEST(LaplaceTest, divgrad)
{
    auto tri_mesh = icosphere();
    EXPECT_LT(div_grad_error(tri_mesh), 1e-13);

    auto quad_mesh = quad_sphere();
    EXPECT_LT(div_grad_error(quad_mesh), 1e-13);
}

TEST(LaplaceTest, constant_gradients)
{
    auto tri_mesh = plane();
    triangulate(tri_mesh);
    EXPECT_LT(constant_gradient_error(tri_mesh), 1e-14);

    auto quad_mesh = plane();
    EXPECT_LT(constant_gradient_error(quad_mesh), 1e-14);
}

TEST(LaplaceTest, linear_precision)
{
    auto tri_mesh = plane();
    triangulate(tri_mesh);
    EXPECT_LT(linear_precision_error(tri_mesh), 1e-14);

    auto quad_mesh = plane();
    EXPECT_LT(linear_precision_error(quad_mesh), 1e-14);
}

TEST(LaplaceTest, mass_matrix)
{
    auto tri_mesh = icosphere();
    EXPECT_LT(mass_matrix_error(tri_mesh), 1e-3);

    auto quad_mesh = quad_sphere();
    EXPECT_LT(mass_matrix_error(quad_mesh), 1e-3);
}

TEST(LaplaceTest, degenerate_triangles)
{
    auto mesh = plane();
    triangulate(mesh);
    degenerate_some_edges(mesh, 10, 0);

    EXPECT_LT(eigenvalues_error(mesh), -0.1);
    // EXPECT_LT(constant_gradient_error(mesh), 1e-14);
    // EXPECT_LT(div_grad_error(mesh), 1e-13);
    // EXPECT_LT(linear_precision_error(mesh), 1e-14);
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
