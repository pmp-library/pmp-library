//=============================================================================
// Copyright (C) 2011-2019 The pmp-library developers
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//=============================================================================

#include <pmp/algorithms/SurfaceSmoothing.h>
#include <pmp/algorithms/DifferentialGeometry.h>

#include <Eigen/Dense>
#include <Eigen/Sparse>

//=============================================================================

namespace pmp {

//=============================================================================

using SparseMatrix = Eigen::SparseMatrix<double>;
using Triplet = Eigen::Triplet<double>;

//=============================================================================

void SurfaceSmoothing::explicit_smoothing(unsigned int iters,
                                          bool use_uniform_laplace)
{
    auto points = mesh_.vertex_property<Point>("v:point");
    auto eweight = mesh_.add_edge_property<Scalar>("e:cotan");
    auto laplace = mesh_.add_vertex_property<Point>("v:laplace");

    // compute Laplace weight per edge: cotan or uniform
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
    mesh_.remove_edge_property(eweight);
}

//-----------------------------------------------------------------------------

void SurfaceSmoothing::implicit_smoothing(Scalar timestep,
                                          bool use_uniform_laplace)
{
    if (!mesh_.n_vertices())
        return;

    // properties
    auto points = mesh_.vertex_property<Point>("v:point");
    auto vweight = mesh_.add_vertex_property<Scalar>("v:area");
    auto eweight = mesh_.add_edge_property<Scalar>("e:cotan");
    auto idx = mesh_.add_vertex_property<int>("v:idx", -1);

    // compute weights: cotan or uniform
    if (use_uniform_laplace)
    {
        for (auto v : mesh_.vertices())
            vweight[v] = 1.0 / mesh_.valence(v);
        for (auto e : mesh_.edges())
            eweight[e] = 1.0;
    }
    else
    {
        for (auto v : mesh_.vertices())
            vweight[v] = 0.5 / voronoi_area(mesh_, v);
        for (auto e : mesh_.edges())
            eweight[e] = std::max(0.0, cotan_weight(mesh_, e));
    }

    // collect free (non-boundary) vertices in array free_vertices[]
    // assign indices such that idx[ free_vertices[i] ] == i
    unsigned i = 0;
    std::vector<Vertex> free_vertices;
    free_vertices.reserve(mesh_.n_vertices());
    for (auto v : mesh_.vertices())
    {
        if (!mesh_.is_boundary(v))
        {
            idx[v] = i++;
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
    double ww;
    Vertex v, vv;
    Edge e;
    for (unsigned int i = 0; i < n; ++i)
    {
        v = free_vertices[i];

        // rhs row
        B(i, 0) = points[v][0] / vweight[v];
        B(i, 1) = points[v][1] / vweight[v];
        B(i, 2) = points[v][2] / vweight[v];

        // lhs row
        ww = 0.0;
        for (auto h : mesh_.halfedges(v))
        {
            vv = mesh_.to_vertex(h);
            e = mesh_.edge(h);
            ww += eweight[e];

            // fixed boundary vertex -> right hand side
            if (mesh_.is_boundary(vv))
            {
                B(i, 0) -= -timestep * eweight[e] * points[vv][0];
                B(i, 1) -= -timestep * eweight[e] * points[vv][1];
                B(i, 2) -= -timestep * eweight[e] * points[vv][2];
            }
            // free interior vertex -> matrix
            else
            {
                triplets.emplace_back(i, idx[vv], -timestep * eweight[e]);
            }
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
        std::cerr << "SurfaceSmoothing: Could not solve linear system\n";
    }
    else
    {
        // copy solution
        for (unsigned int i = 0; i < n; ++i)
        {
            v = free_vertices[i];
            points[v][0] = X(i, 0);
            points[v][1] = X(i, 1);
            points[v][2] = X(i, 2);
        }
    }

    // clean-up
    mesh_.remove_vertex_property(idx);
    mesh_.remove_vertex_property(vweight);
    mesh_.remove_edge_property(eweight);
}

//=============================================================================
} // namespace pmp
//=============================================================================
