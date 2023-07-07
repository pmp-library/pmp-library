// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/algorithms/smoothing.h"
#include "pmp/algorithms/differential_geometry.h"
#include "pmp/algorithms/laplace.h"

namespace pmp {

void explicit_smoothing(SurfaceMesh& mesh, unsigned int iters,
                        bool use_uniform_laplace)
{
    if (!mesh.n_vertices())
        return;

    const unsigned int n = mesh.n_vertices();
    auto points = mesh.get_vertex_property<Point>("v:point");

    // Laplace matrix (clamp negative cotan weights to zero)
    SparseMatrix L;
    setup_laplace_matrix(mesh, L, use_uniform_laplace, true);

    // normalize each row by sum of weights
    // scale by 0.5 to make it more robust
    // multiply by -1 to make it neg. definite again
    L = -0.5 * L.diagonal().asDiagonal().inverse() * L;

    // cancel out boundary vertices
    SparseMatrix S;
    auto is_inner = [&](Vertex v) { return !mesh.is_boundary(v); };
    setup_selector_matrix(mesh, is_inner, S);
    L = S.transpose() * S * L;

    DenseMatrix X(n, 3);

    // copy vertex coordinates to matrix
    for (auto v : mesh.vertices())
        X.row(v.idx()) = static_cast<Eigen::Vector3d>(points[v]);

    // perform some interations
    for (unsigned int i = 0; i < iters; ++i)
        X += L * X;

    // copy matrix back to vertex coordinates
    for (auto v : mesh.vertices())
        points[v] = X.row(v.idx());
}

void implicit_smoothing(SurfaceMesh& mesh, Scalar timestep,
                        bool use_uniform_laplace, bool rescale)
{
    if (!mesh.n_vertices())
        return;

    const unsigned int n = mesh.n_vertices();
    auto points = mesh.get_vertex_property<Point>("v:point");

    // store center and area
    Point center_before(0, 0, 0);
    Scalar area_before(0);
    if (rescale)
    {
        center_before = centroid(mesh);
        area_before = surface_area(mesh);
    }

    // build system matrix A (clamp negative cotan weights to zero)
    SparseMatrix L;
    setup_laplace_matrix(mesh, L, use_uniform_laplace, true);
    DiagonalMatrix M;
    setup_mass_matrix(mesh, M, use_uniform_laplace);
    SparseMatrix A = SparseMatrix(M) - timestep * L;

    // build right-hand side B
    DenseMatrix X(n, 3);
    for (auto v : mesh.vertices())
        X.row(v.idx()) = static_cast<Eigen::Vector3d>(points[v]);
    DenseMatrix B = M * X;

    // solve system
    auto is_constrained = [&](unsigned int i) {
        return mesh.is_boundary(Vertex(i));
    };
    X = cholesky_solve(A, B, is_constrained, X);
    for (auto v : mesh.vertices())
        points[v] = X.row(v.idx());

    if (rescale)
    {
        // restore original surface area
        Scalar area_after = surface_area(mesh);
        Scalar scale = sqrt(area_before / area_after);
        for (auto v : mesh.vertices())
            mesh.position(v) *= scale;

        // restore original center
        Point center_after = centroid(mesh);
        Point trans = center_before - center_after;
        for (auto v : mesh.vertices())
            mesh.position(v) += trans;
    }
}

} // namespace pmp
