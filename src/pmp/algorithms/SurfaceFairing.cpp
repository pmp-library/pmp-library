//=============================================================================
// Copyright (C) 2011-2018 The pmp-library developers
// All rights reserved.
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

SurfaceFairing::SurfaceFairing(SurfaceMesh& mesh) : m_mesh(mesh)
{
    // get & add properties
    m_points    = m_mesh.vertexProperty<Point>("v:point");
    m_vselected = m_mesh.getVertexProperty<bool>("v:selected");
    m_vlocked   = m_mesh.addVertexProperty<bool>("fairing:locked");
    m_vweight   = m_mesh.addVertexProperty<double>("fairing:vweight");
    m_eweight   = m_mesh.addEdgeProperty<double>("fairing:eweight");
    m_idx       = m_mesh.addVertexProperty<int>("fairing:idx", -1);
}

//-----------------------------------------------------------------------------

SurfaceFairing::~SurfaceFairing()
{
    // remove properties
    m_mesh.removeVertexProperty(m_vlocked);
    m_mesh.removeVertexProperty(m_vweight);
    m_mesh.removeEdgeProperty(m_eweight);
    m_mesh.removeVertexProperty(m_idx);
}

//-----------------------------------------------------------------------------

void SurfaceFairing::fair(unsigned int k)
{
    // compute cotan weights
    for (auto v : m_mesh.vertices())
    {
        m_vweight[v] = 0.5 / voronoiArea(m_mesh, v);
    }
    for (auto e : m_mesh.edges())
    {
        m_eweight[e] = std::max(0.0, cotanWeight(m_mesh, e));
    }

    // check whether some vertices are selected
    bool noSelection = true;
    if (m_vselected)
    {
        for (auto v : m_mesh.vertices())
        {
            if (m_vselected[v])
            {
                noSelection = false;
                break;
            }
        }
    }

    // lock k locked boundary rings
    for (auto v : m_mesh.vertices())
    {
        // lock boundary
        if (m_mesh.isSurfaceBoundary(v))
        {
            m_vlocked[v] = true;

            // lock one-ring of boundary
            if (k > 1)
            {
                for (auto vv : m_mesh.vertices(v))
                {
                    m_vlocked[vv] = true;

                    // lock two-ring of boundary
                    if (k > 2)
                    {
                        for (auto vvv : m_mesh.vertices(vv))
                        {
                            m_vlocked[vvv] = true;
                        }
                    }
                }
            }
        }
    }

    // lock un-selected and isolated vertices
    for (auto v : m_mesh.vertices())
    {
        if (!noSelection && !m_vselected[v])
        {
            m_vlocked[v] = true;
        }

        if (m_mesh.isIsolated(v))
        {
            m_vlocked[v] = true;
        }
    }

    // collect free vertices
    std::vector<SurfaceMesh::Vertex> vertices;
    vertices.reserve(m_mesh.nVertices());
    for (auto v : m_mesh.vertices())
    {
        if (!m_vlocked[v])
        {
            m_idx[v] = vertices.size();
            vertices.push_back(v);
        }
    }

    // construct matrix & rhs
    const unsigned int n = vertices.size();
    SparseMatrix       A(n, n);
    Eigen::MatrixXd    B(n, 3);

    std::map<SurfaceMesh::Vertex, double> row;
    std::vector<Triplet> triplets;

    for (unsigned int i = 0; i < n; ++i)
    {
        B(i, 0) = 0.0;
        B(i, 1) = 0.0;
        B(i, 2) = 0.0;

        setupMatrixRow(vertices[i], m_vweight, m_eweight, k, row);

        for (auto r : row)
        {
            auto v = r.first;
            auto w = r.second;

            if (m_idx[v] != -1)
            {
                triplets.emplace_back(i, m_idx[v], w);
            }
            else
            {
                B(i, 0) -= w * m_points[v][0];
                B(i, 1) -= w * m_points[v][1];
                B(i, 2) -= w * m_points[v][2];
            }
        }
    }

    A.setFromTriplets(triplets.begin(), triplets.end());

    // solve A*X = B
    Eigen::SimplicialLDLT<SparseMatrix> solver(A);
    Eigen::MatrixXd                     X = solver.solve(B);

    if (solver.info() != Eigen::Success)
    {
        std::cerr << "SurfaceFairing: Could not solve linear system\n";
    }
    else
    {
        for (unsigned int i = 0; i < n; ++i)
        {
            auto v      = vertices[i];
            m_points[v] = Point(X(m_idx[v], 0), X(m_idx[v], 1), X(m_idx[v], 2));
        }
    }
}

//-----------------------------------------------------------------------------

struct Triple
{
    Triple() = default;

    Triple(SurfaceMesh::Vertex v, double weight, unsigned int degree)
        : m_v(v), m_weight(weight), m_degree(degree)
    {
    }

    SurfaceMesh::Vertex m_v;
    double              m_weight;
    unsigned int        m_degree;
};

//-----------------------------------------------------------------------------

void SurfaceFairing::setupMatrixRow(const SurfaceMesh::Vertex           v,
                                    SurfaceMesh::VertexProperty<double> vweight,
                                    SurfaceMesh::EdgeProperty<double>   eweight,
                                    unsigned int laplaceDegree,
                                    std::map<SurfaceMesh::Vertex, double>& row)
{
    Triple t(v, 1.0, laplaceDegree);

    // init
    static std::vector<Triple> todo;
    todo.reserve(50);
    todo.push_back(t);
    row.clear();

    while (!todo.empty())
    {
        t = todo.back();
        todo.pop_back();
        auto v = t.m_v;
        auto d = t.m_degree;

        if (d == 0)
        {
            row[v] += t.m_weight;
        }

        // else if (d == 1 && m_mesh.isSurfaceBoundary(v))
        // {
        //     // ignore?
        // }

        else
        {
            auto ww = 0.0;

            for (auto h : m_mesh.halfedges(v))
            {
                auto e  = m_mesh.edge(h);
                auto vv = m_mesh.toVertex(h);
                auto w  = eweight[e];

                if (d < laplaceDegree)
                    w *= vweight[v];

                w *= t.m_weight;
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
