// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/algorithms/Smoothing.h"

#include <Eigen/Dense>
#include <Eigen/Sparse>

#include "pmp/algorithms/DifferentialGeometry.h"

namespace pmp {

using SparseMatrix = Eigen::SparseMatrix<double>;
using Triplet = Eigen::Triplet<double>;

Smoothing::Smoothing(SurfaceMesh& mesh) : mesh_(mesh) {}

Smoothing::~Smoothing()
{
    auto vweight = mesh_.get_vertex_property<Scalar>("v:area");
    if (vweight)
        mesh_.remove_vertex_property(vweight);

    auto eweight = mesh_.get_edge_property<Scalar>("e:cotan");
    if (eweight)
        mesh_.remove_edge_property(eweight);
}

void Smoothing::compute_edge_weights(bool use_uniform_laplace)
{
    auto eweight = mesh_.edge_property<Scalar>("e:cotan");

    if (use_uniform_laplace)
    {
        for (auto e : mesh_.edges())
            eweight[e] = 1.0;
    }
    else
    {
        for (auto e : mesh_.edges())
            eweight[e] = std::max(0.0, cotan_weight(mesh_, e));
    }

    n_edge_weights_ = mesh_.n_edges();
}

void Smoothing::compute_vertex_weights(bool use_uniform_laplace)
{
    auto vweight = mesh_.vertex_property<Scalar>("v:area");

    if (use_uniform_laplace)
    {
        for (auto v : mesh_.vertices())
            vweight[v] = 1.0 / mesh_.valence(v);
    }
    else
    {
        for (auto v : mesh_.vertices())
            vweight[v] = 0.5 / voronoi_area(mesh_, v);
    }

    n_vertex_weights_ = mesh_.n_vertices();
}

void Smoothing::explicit_smoothing(unsigned int iters, bool use_uniform_laplace)
{
    if (!mesh_.n_vertices())
        return;

    // TODO: this is calling for trouble: `use_uniform_laplace` might not be respected at all!
    // compute Laplace weight per edge: cotan or uniform
    if (!mesh_.has_edge_property("e:cotan") ||
        n_edge_weights_ != mesh_.n_edges())
        compute_edge_weights(use_uniform_laplace);

    auto points = mesh_.get_vertex_property<Point>("v:point");
    auto eweight = mesh_.get_edge_property<Scalar>("e:cotan");
    auto laplace = mesh_.add_vertex_property<Point>("v:laplace");

    // smoothing iterations
    Vertex vv;
    Edge e;
    for (unsigned int i = 0; i < iters; ++i)
    {
        // step 1: compute Laplace for each vertex
        for (auto v : mesh_.vertices())
        {
            Point l(0, 0, 0);

            if (!mesh_.is_boundary(v))
            {
                Scalar w(0);

                for (auto h : mesh_.halfedges(v))
                {
                    vv = mesh_.to_vertex(h);
                    e = mesh_.edge(h);
                    l += eweight[e] * (points[vv] - points[v]);
                    w += eweight[e];
                }

                l /= w;
            }

            laplace[v] = l;
        }

        // step 2: move each vertex by its (damped) Laplacian
        for (auto v : mesh_.vertices())
        {
            points[v] += 0.5f * laplace[v];
        }
    }

    // clean-up custom properties
    mesh_.remove_vertex_property(laplace);
}

void Smoothing::implicit_smoothing(Scalar timestep, bool use_uniform_laplace,
                                   bool rescale)
{
    if (!mesh_.n_vertices())
        return;

    // TODO: this is calling for trouble: `use_uniform_laplace` might not be respected at all!
    // compute edge weights if they don't exist or if the mesh changed
    if (!mesh_.has_edge_property("e:cotan") ||
        n_edge_weights_ != mesh_.n_edges())
        compute_edge_weights(use_uniform_laplace);

    // compute vertex weights
    compute_vertex_weights(use_uniform_laplace);

    // store center and area
    Point center_before(0, 0, 0);
    Scalar area_before(0);
    if (rescale)
    {
        center_before = centroid(mesh_);
        area_before = surface_area(mesh_);
    }

    // properties
    auto points = mesh_.get_vertex_property<Point>("v:point");
    auto vweight = mesh_.get_vertex_property<Scalar>("v:area");
    auto eweight = mesh_.get_edge_property<Scalar>("e:cotan");
    auto idx = mesh_.add_vertex_property<int>("v:idx", -1);

    // collect free (non-boundary) vertices in array free_vertices[]
    // assign indices such that idx[ free_vertices[j] ] == j
    unsigned j = 0;
    std::vector<Vertex> free_vertices;
    free_vertices.reserve(mesh_.n_vertices());
    for (auto v : mesh_.vertices())
    {
        if (!mesh_.is_boundary(v))
        {
            idx[v] = j++;
            free_vertices.push_back(v);
        }
    }
    const unsigned int n = free_vertices.size();

    // A*X = B
    SparseMatrix A(n, n);
    Eigen::MatrixXd B(n, 3);

    // nonzero elements of A as triplets: (row, column, value)
    std::vector<Triplet> triplets;

    // setup matrix A and rhs B
    for (unsigned int i = 0; i < n; ++i)
    {
        auto v = free_vertices[i];

        // rhs row
        auto b = static_cast<dvec3>(points[v]) / vweight[v];

        // lhs row
        auto ww = 0.0;
        for (auto h : mesh_.halfedges(v))
        {
            auto vv = mesh_.to_vertex(h);
            auto e = mesh_.edge(h);
            ww += eweight[e];

            // fixed boundary vertex -> right hand side
            if (mesh_.is_boundary(vv))
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
        mesh_.remove_vertex_property(idx);
        auto what = "Smoothing: Failed to solve linear system.";
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
        Scalar area_after = surface_area(mesh_);
        Scalar scale = sqrt(area_before / area_after);
        for (auto v : mesh_.vertices())
            mesh_.position(v) *= scale;

        // restore original center
        Point center_after = centroid(mesh_);
        Point trans = center_before - center_after;
        for (auto v : mesh_.vertices())
            mesh_.position(v) += trans;
    }

    // clean-up
    mesh_.remove_vertex_property(idx);
}

} // namespace pmp
