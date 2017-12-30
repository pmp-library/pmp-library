//=============================================================================
// Copyright (C) 2011-2017 The pmp-library developers
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

#include <pmp/algorithms/SurfaceParametrization.h>
#include <pmp/algorithms/DifferentialGeometry.h>
#include <cmath>
#include <Eigen/Dense>
#include <Eigen/Sparse>

//=============================================================================

namespace pmp {

//=============================================================================

SurfaceParameterization::SurfaceParameterization(SurfaceMesh& mesh) : m_mesh(mesh)
{
}

//-----------------------------------------------------------------------------

bool SurfaceParameterization::setupBoundaryConstraints()
{
    // get properties
    auto points = m_mesh.vertexProperty<Point>("v:point");
    auto tex    = m_mesh.vertexProperty<TextureCoordinate>("v:tex");

    SurfaceMesh::VertexIterator      vit, vend = m_mesh.verticesEnd();
    SurfaceMesh::Vertex              vh;
    SurfaceMesh::Halfedge            hh;
    std::vector<SurfaceMesh::Vertex> loop;

    // Initialize all texture coordinates to the origin.
    for (auto v : m_mesh.vertices())
        tex[v] = TextureCoordinate(0.5, 0.5);

    // find 1st boundary vertex
    for (vit = m_mesh.verticesBegin(); vit != vend; ++vit)
        if (m_mesh.isBoundary(*vit))
            break;

    // no boundary found ?
    if (vit == vend)
    {
        std::cerr << "Mesh has no boundary." << std::endl;
        return false;
    }

    // collect boundary loop
    vh = *vit;
    hh = m_mesh.halfedge(vh);
    do
    {
        loop.push_back(m_mesh.toVertex(hh));
        hh = m_mesh.nextHalfedge(hh);
    } while (hh != m_mesh.halfedge(vh));

    // map boundary loop to unit circle in texture domain
    unsigned int      i, n = loop.size();
    Scalar            angle, l, length;
    TextureCoordinate t;

    // compute length of boundary loop
    for (i = 0, length = 0.0; i < n; ++i)
        length += distance(points[loop[i]], points[loop[(i + 1) % n]]);

    // map length intervalls to unit circle intervals
    for (i = 0, l = 0.0; i < n;)
    {
        // go from 2pi to 0 to preserve orientation
        angle = 2.0 * M_PI * (1.0 - l / length);

        t[0] = 0.5 + 0.5 * cosf(angle);
        t[1] = 0.5 + 0.5 * sinf(angle);

        tex[loop[i]] = t;

        ++i;
        if (i < n)
        {
            l += distance(points[loop[i]], points[loop[(i + 1) % n]]);
        }
    }

    return true;
}

//-----------------------------------------------------------------------------

void SurfaceParameterization::parameterize(bool uniform)
{
    // get properties
    auto tex     = m_mesh.vertexProperty<TextureCoordinate>("v:tex");
    auto eweight = m_mesh.edgeProperty<Scalar>("e:cotan");

    // compute Laplace weight per edge: cotan or uniform
    for (auto e : m_mesh.edges())
    {
        eweight[e] = uniform ? 1.0 : std::max(0.0, cotanWeight(m_mesh, e));
    }

    // map boundary to circle
    if (!setupBoundaryConstraints())
    {
        std::cerr
            << "Could not perform setup of boundary constraints.\nExiting..."
            << std::endl;
        return;
    }

    // collect free (non-boundary) vertices in array free_vertices[]
    // assign indices such that idx[ free_vertices[i] ] == i
    unsigned i   = 0;
    auto     idx = m_mesh.addVertexProperty<int>("v:idx", -1);
    std::vector<SurfaceMesh::Vertex> free_vertices;
    free_vertices.reserve(m_mesh.nVertices());
    for (auto v : m_mesh.vertices())
    {
        if (!m_mesh.isBoundary(v))
        {
            idx[v] = i++;
            free_vertices.push_back(v);
        }
    }

    // setup matrix A and rhs B
    const unsigned int                  n = free_vertices.size();
    Eigen::SparseMatrix<double>         A(n, n);
    Eigen::MatrixXd                     B(n, 2);
    std::vector<Eigen::Triplet<double>> triplets;
    double                              w, ww;
    SurfaceMesh::Vertex                 v, vv;
    SurfaceMesh::Edge                   e;
    for (i = 0; i < n; ++i)
    {
        v = free_vertices[i];

        // rhs row
        B(i, 0) = 0.0;
        B(i, 1) = 0.0;

        // lhs row
        ww = 0.0;
        for (auto h : m_mesh.halfedges(v))
        {
            vv = m_mesh.toVertex(h);
            e  = m_mesh.edge(h);
            w  = eweight[e];
            ww += w;

            if (m_mesh.isBoundary(vv))
            {
                B(i, 0) -= -w * tex[vv][0];
                B(i, 1) -= -w * tex[vv][1];
            }
            else
            {
                triplets.push_back(Eigen::Triplet<double>(i, idx[vv], -w));
            }
        }
        triplets.push_back(Eigen::Triplet<double>(i, i, ww));
    }

    // build sparse matrix from triplets
    A.setFromTriplets(triplets.begin(), triplets.end());

    // solve A*X = B
    Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> solver(A);
    Eigen::MatrixXd                                    X = solver.solve(B);
    if (solver.info() != Eigen::Success)
    {
        std::cerr << "SurfaceParameterization: Could not solve linear system\n";
    }
    else
    {
        // copy solution
        for (i = 0; i < n; ++i)
        {
            v         = free_vertices[i];
            tex[v][0] = X(i, 0);
            tex[v][1] = X(i, 1);
        }
    }

    // clean-up
    m_mesh.removeVertexProperty(idx);
}

//=============================================================================
} // namespace pmp
//=============================================================================
