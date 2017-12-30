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

#include <pmp/algorithms/SurfaceParameterization.h>
#include <pmp/algorithms/DifferentialGeometry.h>
#include <cmath>
#include <Eigen/Dense>
#include <Eigen/Sparse>

//=============================================================================

namespace pmp {

//=============================================================================

SurfaceParameterization::SurfaceParameterization(SurfaceMesh& mesh)
    : m_mesh(mesh)
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

void SurfaceParameterization::harmonic(bool uniform)
{
    // map boundary to circle
    if (!setupBoundaryConstraints())
    {
        std::cerr << "Could not perform setup of boundary constraints.\n";
        return;
    }

    // get properties
    auto tex     = m_mesh.vertexProperty<TextureCoordinate>("v:tex");
    auto eweight = m_mesh.addEdgeProperty<Scalar>("e:param");
    auto idx     = m_mesh.addVertexProperty<int>("v:idx", -1);

    // compute Laplace weight per edge: cotan or uniform
    for (auto e : m_mesh.edges())
    {
        eweight[e] = uniform ? 1.0 : std::max(0.0, cotanWeight(m_mesh, e));
    }

    // collect free (non-boundary) vertices in array free_vertices[]
    // assign indices such that idx[ free_vertices[i] ] == i
    unsigned                         i = 0;
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
    m_mesh.removeEdgeProperty(eweight);
}

//-----------------------------------------------------------------------------

bool SurfaceParameterization::setupLSCMBoundary()
{
    // constrain the two boundary vertices farthest from each other to fix
    // the translation and rotation of the resulting parameterization

    // vertex properties
    auto pos    = m_mesh.vertexProperty<Point>("v:point");
    auto tex    = m_mesh.vertexProperty<TexCoord>("v:tex");
    auto locked = m_mesh.addVertexProperty<bool>("v:locked", false);

    // find boundary vertices and store handles in vector
    std::vector<SurfaceMesh::Vertex> boundary;
    for (auto v : m_mesh.vertices())
        if (m_mesh.isBoundary(v))
            boundary.push_back(v);

    // no boundary?
    if (boundary.empty())
    {
        return false;
    }

    // find boundary vertices with largest distance
    Scalar              diam(0.0), d;
    SurfaceMesh::Vertex v1, v2;
    for (auto vv1 : boundary)
    {
        for (auto vv2 : boundary)
        {
            d = distance(pos[vv1], pos[vv2]);
            if (d > diam)
            {
                diam = d;
                v1   = vv1;
                v2   = vv2;
            }
        }
    }

    // pin these two boundary vertices
    for (auto v : m_mesh.vertices())
    {
        tex[v]    = TexCoord(0.5, 0.5);
        locked[v] = false;
    }
    tex[v1]    = TexCoord(0.0, 0.0);
    tex[v2]    = TexCoord(1.0, 1.0);
    locked[v1] = true;
    locked[v2] = true;

    return true;
}

//-----------------------------------------------------------------------------

void SurfaceParameterization::lscm()
{
    // boundary constraints
    if (!setupLSCMBoundary())
        return;

    // properties
    auto pos    = m_mesh.vertexProperty<Point>("v:point");
    auto tex    = m_mesh.vertexProperty<TexCoord>("v:tex");
    auto idx    = m_mesh.addVertexProperty<int>("v:idx", -1);
    auto weight = m_mesh.addHalfedgeProperty<dvec2>("h:lscm");
    auto locked = m_mesh.getVertexProperty<bool>("v:locked");
    assert(locked);

    // compute weights/gradients per face/halfedge
    for (auto f : m_mesh.faces())
    {
        // collect face halfedge
        auto fh_it = m_mesh.halfedges(f);
        auto ha    = *fh_it;
        ++fh_it;
        auto hb = *fh_it;
        ++fh_it;
        auto hc = *fh_it;

        // collect face vertices
        dvec3 a = (dvec3)pos[m_mesh.toVertex(ha)];
        dvec3 b = (dvec3)pos[m_mesh.toVertex(hb)];
        dvec3 c = (dvec3)pos[m_mesh.toVertex(hc)];

        // calculate local coordinate system
        dvec3 z = cross(c - b, a - b);
        dvec3 x = normalize(b - a);
        dvec3 y = normalize(cross(z, x));

        // calculate local vertex coordinates
        dvec2 a2D(0.0, 0.0);
        dvec2 b2D(norm(b - a), 0.0);
        dvec2 c2D(dot(c - a, x), dot(c - a, y));

        // calculate double triangle area
        double area = norm(z);
        if (area)
            area = 1.0 / area;

        // calculate W_j,Ti (index by corner a,b,c and real/imaginary)
        double War = c2D[0] - b2D[0];
        double Wbr = a2D[0] - c2D[0];
        double Wcr = b2D[0] - a2D[0];
        double Wai = c2D[1] - b2D[1];
        double Wbi = a2D[1] - c2D[1];
        double Wci = b2D[1] - a2D[1];

        // store matrix information per halfedge
        weight[ha] = dvec2(War * area, Wai * area);
        weight[hb] = dvec2(Wbr * area, Wbi * area);
        weight[hc] = dvec2(Wcr * area, Wci * area);
    }

    // collect free (non-boundary) vertices in array free_vertices[]
    // assign indices such that idx[ free_vertices[i] ] == i
    unsigned                         i = 0;
    std::vector<SurfaceMesh::Vertex> free_vertices;
    free_vertices.reserve(m_mesh.nVertices());
    for (auto v : m_mesh.vertices())
    {
        if (!locked[v])
        {
            idx[v] = i++;
            free_vertices.push_back(v);
        }
    }

    // build matrix and rhs
    const unsigned int    nV2 = 2 * m_mesh.nVertices();
    const unsigned int    nV  = m_mesh.nVertices();
    const unsigned int    N   = free_vertices.size();
    SurfaceMesh::Vertex   vi, vj;
    SurfaceMesh::Halfedge hh;
    double                si, sj0, sj1, sign;
    int                   row(0), c0, c1;

    Eigen::SparseMatrix<double>         A(2 * N, 2 * N);
    Eigen::VectorXd                     b = Eigen::VectorXd::Zero(2 * N);
    std::vector<Eigen::Triplet<double>> triplets;

    for (unsigned int i = 0; i < nV2; ++i)
    {
        vi = SurfaceMesh::Vertex(i % nV);

        if (i < nV)
        {
            sign = 1.0;
            c0   = 0;
            c1   = 1;
        }
        else
        {
            sign = -1.0;
            c0   = 1;
            c1   = 0;
        }

        if (!locked[vi])
        {
            si = 0;

            for (auto h : m_mesh.halfedges(vi))
            {
                vj  = m_mesh.toVertex(h);
                sj0 = sj1 = 0;

                if (!m_mesh.isBoundary(h))
                {
                    const dvec2& wj = weight[h];
                    const dvec2& wi = weight[m_mesh.prevHalfedge(h)];

                    sj0 += sign * wi[c0] * wj[0] + wi[c1] * wj[1];
                    sj1 += -sign * wi[c0] * wj[1] + wi[c1] * wj[0];
                    si += wi[0] * wi[0] + wi[1] * wi[1];
                }

                h = m_mesh.oppositeHalfedge(h);
                if (!m_mesh.isBoundary(h))
                {
                    const dvec2& wi = weight[h];
                    const dvec2& wj = weight[m_mesh.prevHalfedge(h)];

                    sj0 += sign * wi[c0] * wj[0] + wi[c1] * wj[1];
                    sj1 += -sign * wi[c0] * wj[1] + wi[c1] * wj[0];
                    si += wi[0] * wi[0] + wi[1] * wi[1];
                }

                if (!locked[vj])
                {
                    triplets.push_back(
                        Eigen::Triplet<double>(row, idx[vj], sj0));
                    triplets.push_back(
                        Eigen::Triplet<double>(row, idx[vj] + N, sj1));
                }
                else
                {
                    b[row] -= sj0 * tex[vj][0];
                    b[row] -= sj1 * tex[vj][1];
                }
            }

            triplets.push_back(Eigen::Triplet<double>(
                row, idx[vi] + (i < nV ? 0 : N), 0.5 * si));

            ++row;
        }
    }

    // build sparse matrix from triplets
    A.setFromTriplets(triplets.begin(), triplets.end());

    // solve A*X = B
    Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> solver(A);
    Eigen::VectorXd                                    x = solver.solve(b);
    if (solver.info() != Eigen::Success)
    {
        std::cerr << "SurfaceParameterization: Could not solve linear system\n";
    }
    else
    {
        // copy solution
        for (i = 0; i < N; ++i)
        {
            tex[free_vertices[i]] = TexCoord(x[i], x[i + N]);
        }
    }

    // scale tex coordiantes to unit square
    TexCoord bbmin(1, 1), bbmax(0, 0);
    for (auto v : m_mesh.vertices())
    {
        bbmin = min(bbmin, tex[v]);
        bbmax = max(bbmax, tex[v]);
    }
    bbmax -= bbmin;
    Scalar s = std::max(bbmax[0], bbmax[1]);
    for (auto v : m_mesh.vertices())
    {
        tex[v] -= bbmin;
        tex[v] /= s;
    }

    // clean-up
    m_mesh.removeVertexProperty(idx);
    m_mesh.removeVertexProperty(locked);
    m_mesh.removeHalfedgeProperty(weight);
}

//=============================================================================
} // namespace pmp
//=============================================================================
