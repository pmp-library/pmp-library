// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/algorithms/smoothing.h"

#include <Eigen/Dense>
#include <Eigen/Sparse>

#include "pmp/algorithms/DifferentialGeometry.h"

namespace pmp {
namespace {

// compute cotan/uniform Laplace weights.
void compute_edge_weights(SurfaceMesh& mesh, bool use_uniform_laplace)
{
    auto eweight = mesh.edge_property<Scalar>("e:cotan");

    if (use_uniform_laplace)
    {
        for (auto e : mesh.edges())
            eweight[e] = 1.0;
    }
    else
    {
        for (auto e : mesh.edges())
            eweight[e] = std::max(0.0, cotan_weight(mesh, e));
    }
}

// compute cotan/uniform Laplace weights.
void compute_vertex_weights(SurfaceMesh& mesh, bool use_uniform_laplace)
{
    auto vweight = mesh.vertex_property<Scalar>("v:area");

    if (use_uniform_laplace)
    {
        for (auto v : mesh.vertices())
            vweight[v] = 1.0 / mesh.valence(v);
    }
    else
    {
        for (auto v : mesh.vertices())
            vweight[v] = 0.5 / voronoi_area(mesh, v);
    }
}
} // namespace

void explicit_smoothing(SurfaceMesh& mesh, unsigned int iters,
                        bool use_uniform_laplace)
{
    if (!mesh.n_vertices())
        return;

    compute_edge_weights(mesh, use_uniform_laplace);

    auto points = mesh.get_vertex_property<Point>("v:point");
    auto eweight = mesh.get_edge_property<Scalar>("e:cotan");
    auto laplace = mesh.add_vertex_property<Point>("v:laplace");

    // smoothing iterations
    Vertex vv;
    Edge e;
    for (unsigned int i = 0; i < iters; ++i)
    {
        // step 1: compute Laplace for each vertex
        for (auto v : mesh.vertices())
        {
            Point l(0, 0, 0);

            if (!mesh.is_boundary(v))
            {
                Scalar w(0);

                for (auto h : mesh.halfedges(v))
                {
                    vv = mesh.to_vertex(h);
                    e = mesh.edge(h);
                    l += eweight[e] * (points[vv] - points[v]);
                    w += eweight[e];
                }

                l /= w;
            }

            laplace[v] = l;
        }

        // step 2: move each vertex by its (damped) Laplacian
        for (auto v : mesh.vertices())
        {
            points[v] += 0.5f * laplace[v];
        }
    }

    // clean-up custom properties
    mesh.remove_vertex_property(laplace);
    mesh.remove_edge_property(eweight);
}

void implicit_smoothing(SurfaceMesh& mesh, Scalar timestep,
                        bool use_uniform_laplace, bool rescale)
{
    if (!mesh.n_vertices())
        return;

    compute_edge_weights(mesh, use_uniform_laplace);
    compute_vertex_weights(mesh, use_uniform_laplace);

    // store center and area
    Point center_before(0, 0, 0);
    Scalar area_before(0);
    if (rescale)
    {
        center_before = centroid(mesh);
        area_before = surface_area(mesh);
    }

    // properties
    auto points = mesh.get_vertex_property<Point>("v:point");
    auto vweight = mesh.get_vertex_property<Scalar>("v:area");
    auto eweight = mesh.get_edge_property<Scalar>("e:cotan");
    auto idx = mesh.add_vertex_property<int>("v:idx", -1);

    // collect free (non-boundary) vertices in array free_vertices[]
    // assign indices such that idx[ free_vertices[j] ] == j
    unsigned j = 0;
    std::vector<Vertex> free_vertices;
    free_vertices.reserve(mesh.n_vertices());
    for (auto v : mesh.vertices())
    {
        if (!mesh.is_boundary(v))
        {
            idx[v] = j++;
            free_vertices.push_back(v);
        }
    }
    const unsigned int n = free_vertices.size();

    // A*X = B
    using SparseMatrix = Eigen::SparseMatrix<double>;
    SparseMatrix A(n, n);
    Eigen::MatrixXd B(n, 3);

    // nonzero elements of A as triplets: (row, column, value)
    using Triplet = Eigen::Triplet<double>;
    std::vector<Triplet> triplets;

    // setup matrix A and rhs B
    for (unsigned int i = 0; i < n; ++i)
    {
        auto v = free_vertices[i];

        // rhs row
        auto b = static_cast<dvec3>(points[v]) / vweight[v];

        // lhs row
        auto ww = 0.0;
        for (auto h : mesh.halfedges(v))
        {
            auto vv = mesh.to_vertex(h);
            auto e = mesh.edge(h);
            ww += eweight[e];

            // fixed boundary vertex -> right hand side
            if (mesh.is_boundary(vv))
            {
                b -= -timestep * eweight[e] * static_cast<dvec3>(points[vv]);
            }
            // free interior vertex -> matrix
            else
            {
                triplets.emplace_back(i, idx[vv], -timestep * eweight[e]);
            }

            B.row(i) = (Eigen::Vector3d)b;
        }

        // center vertex -> matrix
        triplets.emplace_back(i, i, 1.0 / vweight[v] + timestep * ww);
    }

    // build sparse matrix from triplets
    A.setFromTriplets(triplets.begin(), triplets.end());

    // solve A*X = B
    Eigen::SimplicialLDLT<SparseMatrix> solver(A);
    Eigen::MatrixXd X = solver.solve(B);
    if (solver.info() != Eigen::Success)
    {
        // clean-up
        mesh.remove_vertex_property(idx);
        mesh.remove_vertex_property(vweight);
        mesh.remove_edge_property(eweight);
        auto what = std::string{__func__} + ": Failed to solve linear system.";
        throw SolverException(what);
    }
    else
    {
        // copy solution
        for (unsigned int i = 0; i < n; ++i)
        {
            points[free_vertices[i]] = X.row(i);
        }
    }

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

    // clean-up
    mesh.remove_vertex_property(idx);
    mesh.remove_vertex_property(vweight);
    mesh.remove_edge_property(eweight);
}

} // namespace pmp
