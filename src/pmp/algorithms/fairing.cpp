// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/algorithms/fairing.h"

#include <Eigen/Dense>
#include <Eigen/Sparse>

#include "pmp/algorithms/DifferentialGeometry.h"

namespace pmp {
namespace {

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

void setup_matrix_row(const SurfaceMesh& mesh, const Vertex vertex,
                      VertexProperty<double> vweight,
                      EdgeProperty<double> eweight, unsigned int laplace_degree,
                      std::map<Vertex, double>& row)
{
    Triple t(vertex, 1.0, laplace_degree);

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
        else
        {
            auto ww = 0.0;

            for (auto h : mesh.halfedges(v))
            {
                auto e = mesh.edge(h);
                auto vv = mesh.to_vertex(h);
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
} // namespace

void minimize_area(SurfaceMesh& mesh)
{
    fair(mesh, 1);
}

void minimize_curvature(SurfaceMesh& mesh)
{
    fair(mesh, 2);
}

void fair(SurfaceMesh& mesh, unsigned int k)
{
    // get & add properties
    auto points = mesh.vertex_property<Point>("v:point");
    auto vselected = mesh.get_vertex_property<bool>("v:selected");
    auto vlocked = mesh.add_vertex_property<bool>("fairing:locked");
    auto vweight = mesh.add_vertex_property<double>("fairing:vweight");
    auto eweight = mesh.add_edge_property<double>("fairing:eweight");
    auto idx = mesh.add_vertex_property<int>("fairing:idx", -1);

    auto cleanup = [&]() {
        mesh.remove_vertex_property(vlocked);
        mesh.remove_vertex_property(vweight);
        mesh.remove_edge_property(eweight);
        mesh.remove_vertex_property(idx);
    };

    // compute cotan weights
    for (auto v : mesh.vertices())
    {
        vweight[v] = 0.5 / voronoi_area(mesh, v);
    }
    for (auto e : mesh.edges())
    {
        eweight[e] = std::max(0.0, cotan_weight(mesh, e));
    }

    // check whether some vertices are selected
    bool no_selection = true;
    if (vselected)
    {
        for (auto v : mesh.vertices())
        {
            if (vselected[v])
            {
                no_selection = false;
                break;
            }
        }
    }

    // lock k locked boundary rings
    for (auto v : mesh.vertices())
    {
        // lock boundary
        if (mesh.is_boundary(v))
        {
            vlocked[v] = true;

            // lock one-ring of boundary
            if (k > 1)
            {
                for (auto vv : mesh.vertices(v))
                {
                    vlocked[vv] = true;

                    // lock two-ring of boundary
                    if (k > 2)
                    {
                        for (auto vvv : mesh.vertices(vv))
                        {
                            vlocked[vvv] = true;
                        }
                    }
                }
            }
        }
    }

    // lock un-selected and isolated vertices
    for (auto v : mesh.vertices())
    {
        if (!no_selection && !vselected[v])
        {
            vlocked[v] = true;
        }

        if (mesh.is_isolated(v))
        {
            vlocked[v] = true;
        }
    }

    // collect free vertices
    std::vector<Vertex> vertices;
    vertices.reserve(mesh.n_vertices());
    for (auto v : mesh.vertices())
    {
        if (!vlocked[v])
        {
            idx[v] = vertices.size();
            vertices.push_back(v);
        }
    }

    // we need locked vertices as boundary constraints
    if (vertices.size() == mesh.n_vertices())
    {
        cleanup();
        auto what = "Fairing: Missing boundary constraints.";
        throw InvalidInputException(what);
    }

    // construct matrix & rhs
    const unsigned int n = vertices.size();
    using SparseMatrix = Eigen::SparseMatrix<double>;
    SparseMatrix A(n, n);
    Eigen::MatrixXd B(n, 3);
    dvec3 b;

    std::map<Vertex, double> row;
    using Triplet = Eigen::Triplet<double>;
    std::vector<Triplet> triplets;

    for (unsigned int i = 0; i < n; ++i)
    {
        b = dvec3(0.0);

        setup_matrix_row(mesh, vertices[i], vweight, eweight, k, row);

        for (auto r : row)
        {
            auto v = r.first;
            auto w = r.second;

            if (idx[v] != -1)
            {
                triplets.emplace_back(i, idx[v], w);
            }
            else
            {
                b -= w * static_cast<dvec3>(points[v]);
            }
        }

        B.row(i) = (Eigen::Vector3d)b;
    }

    A.setFromTriplets(triplets.begin(), triplets.end());

    // solve A*X = B
    Eigen::SimplicialLDLT<SparseMatrix> solver(A);
    Eigen::MatrixXd X = solver.solve(B);

    if (solver.info() != Eigen::Success)
    {
        throw SolverException("Fairing: Failed to solve linear system.");
    }
    else
    {
        for (unsigned int i = 0; i < n; ++i)
            points[vertices[i]] = X.row(i);
    }

    // remove properties
    cleanup();
}

} // namespace pmp
