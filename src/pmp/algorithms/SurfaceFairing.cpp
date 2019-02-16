//=============================================================================
// Copyright (C) 2011-2019 The pmp-library developers
//
// This file is part of the Polygon Mesh Processing Library.
// Distributed under the terms of the MIT license, see LICENSE.txt for details.
//
// SPDX-License-Identifier: MIT
//=============================================================================

#include <pmp/algorithms/SurfaceFairing.h>
#include <pmp/algorithms/DifferentialGeometry.h>

#include <Eigen/Dense>
#include <Eigen/Sparse>

//=============================================================================

namespace pmp {

//=============================================================================

using SparseMatrix = Eigen::SparseMatrix<double>;
using Triplet = Eigen::Triplet<double>;

//=============================================================================

SurfaceFairing::SurfaceFairing(SurfaceMesh& mesh) : mesh_(mesh)
{
    // get & add properties
    points_ = mesh_.vertex_property<Point>("v:point");
    vselected_ = mesh_.get_vertex_property<bool>("v:selected");
    vlocked_ = mesh_.add_vertex_property<bool>("fairing:locked");
    vweight_ = mesh_.add_vertex_property<double>("fairing:vweight");
    eweight_ = mesh_.add_edge_property<double>("fairing:eweight");
    idx_ = mesh_.add_vertex_property<int>("fairing:idx", -1);
}

//-----------------------------------------------------------------------------

SurfaceFairing::~SurfaceFairing()
{
    // remove properties
    mesh_.remove_vertex_property(vlocked_);
    mesh_.remove_vertex_property(vweight_);
    mesh_.remove_edge_property(eweight_);
    mesh_.remove_vertex_property(idx_);
}

//-----------------------------------------------------------------------------

void SurfaceFairing::fair(unsigned int k)
{
    // compute cotan weights
    for (auto v : mesh_.vertices())
    {
        vweight_[v] = 0.5 / voronoi_area(mesh_, v);
    }
    for (auto e : mesh_.edges())
    {
        eweight_[e] = std::max(0.0, cotan_weight(mesh_, e));
    }

    // check whether some vertices are selected
    bool no_selection = true;
    if (vselected_)
    {
        for (auto v : mesh_.vertices())
        {
            if (vselected_[v])
            {
                no_selection = false;
                break;
            }
        }
    }

    // lock k locked boundary rings
    for (auto v : mesh_.vertices())
    {
        // lock boundary
        if (mesh_.is_boundary(v))
        {
            vlocked_[v] = true;

            // lock one-ring of boundary
            if (k > 1)
            {
                for (auto vv : mesh_.vertices(v))
                {
                    vlocked_[vv] = true;

                    // lock two-ring of boundary
                    if (k > 2)
                    {
                        for (auto vvv : mesh_.vertices(vv))
                        {
                            vlocked_[vvv] = true;
                        }
                    }
                }
            }
        }
    }

    // lock un-selected and isolated vertices
    for (auto v : mesh_.vertices())
    {
        if (!no_selection && !vselected_[v])
        {
            vlocked_[v] = true;
        }

        if (mesh_.is_isolated(v))
        {
            vlocked_[v] = true;
        }
    }

    // collect free vertices
    std::vector<Vertex> vertices;
    vertices.reserve(mesh_.n_vertices());
    for (auto v : mesh_.vertices())
    {
        if (!vlocked_[v])
        {
            idx_[v] = vertices.size();
            vertices.push_back(v);
        }
    }

    // construct matrix & rhs
    const unsigned int n = vertices.size();
    SparseMatrix A(n, n);
    Eigen::MatrixXd B(n, 3);

    std::map<Vertex, double> row;
    std::vector<Triplet> triplets;

    for (unsigned int i = 0; i < n; ++i)
    {
        B(i, 0) = 0.0;
        B(i, 1) = 0.0;
        B(i, 2) = 0.0;

        setup_matrix_row(vertices[i], vweight_, eweight_, k, row);

        for (auto r : row)
        {
            auto v = r.first;
            auto w = r.second;

            if (idx_[v] != -1)
            {
                triplets.emplace_back(i, idx_[v], w);
            }
            else
            {
                B(i, 0) -= w * points_[v][0];
                B(i, 1) -= w * points_[v][1];
                B(i, 2) -= w * points_[v][2];
            }
        }
    }

    A.setFromTriplets(triplets.begin(), triplets.end());

    // solve A*X = B
    Eigen::SimplicialLDLT<SparseMatrix> solver(A);
    Eigen::MatrixXd X = solver.solve(B);

    if (solver.info() != Eigen::Success)
    {
        std::cerr << "SurfaceFairing: Could not solve linear system\n";
    }
    else
    {
        for (unsigned int i = 0; i < n; ++i)
        {
            auto v = vertices[i];
            points_[v] = Point(X(idx_[v], 0), X(idx_[v], 1), X(idx_[v], 2));
        }
    }
}

//-----------------------------------------------------------------------------

struct Triple
{
    Triple() = default;

    Triple(Vertex v, double weight, unsigned int degree)
        : vertex_(v), weight_(weight), degree_(degree)
    {
    }

    Vertex vertex_;
    double weight_;
    unsigned int degree_;
};

//-----------------------------------------------------------------------------

void SurfaceFairing::setup_matrix_row(const Vertex v,
                                      VertexProperty<double> vweight,
                                      EdgeProperty<double> eweight,
                                      unsigned int laplace_degree,
                                      std::map<Vertex, double>& row)
{
    Triple t(v, 1.0, laplace_degree);

    // init
    static std::vector<Triple> todo;
    todo.reserve(50);
    todo.push_back(t);
    row.clear();

    while (!todo.empty())
    {
        t = todo.back();
        todo.pop_back();
        auto v = t.vertex_;
        auto d = t.degree_;

        if (d == 0)
        {
            row[v] += t.weight_;
        }

        // else if (d == 1 && mesh_.is_boundary(v))
        // {
        //     // ignore?
        // }

        else
        {
            auto ww = 0.0;

            for (auto h : mesh_.halfedges(v))
            {
                auto e = mesh_.edge(h);
                auto vv = mesh_.to_vertex(h);
                auto w = eweight[e];

                if (d < laplace_degree)
                    w *= vweight[v];

                w *= t.weight_;
                ww -= w;

                todo.emplace_back(vv, w, d - 1);
            }

            todo.emplace_back(v, ww, d - 1);
        }
    }
}

//=============================================================================
} // namespace pmp
//=============================================================================
