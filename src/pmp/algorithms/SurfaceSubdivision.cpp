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

#include <pmp/algorithms/SurfaceSubdivision.h>

//=============================================================================

namespace pmp {

//=============================================================================

SurfaceSubdivision::SurfaceSubdivision(SurfaceMesh& mesh) : m_mesh(mesh)
{
    m_points   = m_mesh.vertexProperty<Point>("v:point");
    m_vfeature = m_mesh.getVertexProperty<bool>("v:feature");
    m_efeature = m_mesh.getEdgeProperty<bool>("e:feature");
}

//-----------------------------------------------------------------------------

void SurfaceSubdivision::catmullClark()
{
    // reserve memory
    size_t nv = m_mesh.nVertices();
    size_t ne = m_mesh.nEdges();
    size_t nf = m_mesh.nFaces();
    m_mesh.reserve(nv + ne + nf, 2 * ne + 4 * nf, 4 * nf);

    // get properties
    auto vpoint = m_mesh.addVertexProperty<Point>("catmull:vpoint");
    auto epoint = m_mesh.addEdgeProperty<Point>("catmull:epoint");
    auto fpoint = m_mesh.addFaceProperty<Point>("catmull:fpoint");

    // compute face vertices
    for (auto f : m_mesh.faces())
    {
        Point  p(0, 0, 0);
        Scalar c(0);
        for (auto v : m_mesh.vertices(f))
        {
            p += m_points[v];
            ++c;
        }
        p /= c;
        fpoint[f] = p;
    }

    // compute edge vertices
    for (auto e : m_mesh.edges())
    {
        // boundary or feature edge?
        if (m_mesh.isSurfaceBoundary(e) || (m_efeature && m_efeature[e]))
        {
            epoint[e] = 0.5 * (m_points[m_mesh.vertex(e, 0)] +
                               m_points[m_mesh.vertex(e, 1)]);
        }

        // interior edge
        else
        {
            Point p(0, 0, 0);
            p += m_points[m_mesh.vertex(e, 0)];
            p += m_points[m_mesh.vertex(e, 1)];
            p += fpoint[m_mesh.face(e, 0)];
            p += fpoint[m_mesh.face(e, 1)];
            p *= 0.25;
            epoint[e] = p;
        }
    }

    // compute new positions for old vertices
    for (auto v : m_mesh.vertices())
    {
        // isolated vertex?
        if (m_mesh.isIsolated(v))
        {
            vpoint[v] = m_points[v];
        }

        // boundary vertex?
        else if (m_mesh.isSurfaceBoundary(v))
        {
            auto h1 = m_mesh.halfedge(v);
            auto h0 = m_mesh.prevHalfedge(h1);

            Point p = m_points[v];
            p *= 6.0;
            p += m_points[m_mesh.toVertex(h1)];
            p += m_points[m_mesh.fromVertex(h0)];
            p *= 0.125;

            vpoint[v] = p;
        }

        // interior feature vertex?
        else if (m_vfeature && m_vfeature[v])
        {
            Point p = m_points[v];
            p *= 6.0;
            int count(0);

            for (auto h : m_mesh.halfedges(v))
            {
                if (m_efeature[m_mesh.edge(h)])
                {
                    p += m_points[m_mesh.toVertex(h)];
                    ++count;
                }
            }

            if (count == 2) // vertex is on feature edge
            {
                p *= 0.125;
                vpoint[v] = p;
            }
            else // keep fixed
            {
                vpoint[v] = m_points[v];
            }
        }

        // interior vertex
        else
        {
            // weights from SIGGRAPH paper "Subdivision Surfaces in Character Animation"

            const Scalar k = m_mesh.valence(v);
            Point        p(0, 0, 0);

            for (auto vv : m_mesh.vertices(v))
                p += m_points[vv];

            for (auto f : m_mesh.faces(v))
                p += fpoint[f];

            p /= (k * k);

            p += ((k - 2.0) / k) * m_points[v];

            vpoint[v] = p;
        }
    }

    // assign new positions to old vertices
    for (auto v : m_mesh.vertices())
    {
        m_points[v] = vpoint[v];
    }

    // split edges
    for (auto e : m_mesh.edges())
    {
        // feature edge?
        if (m_efeature && m_efeature[e])
        {
            auto h  = m_mesh.insertVertex(e, epoint[e]);
            auto v  = m_mesh.toVertex(h);
            auto e0 = m_mesh.edge(h);
            auto e1 = m_mesh.edge(m_mesh.nextHalfedge(h));

            m_vfeature[v]  = true;
            m_efeature[e0] = true;
            m_efeature[e1] = true;
        }

        // normal edge
        else
        {
            m_mesh.insertVertex(e, epoint[e]);
        }
    }

    // split faces
    for (auto f : m_mesh.faces())
    {
        auto h0 = m_mesh.halfedge(f);
        m_mesh.insertEdge(h0, m_mesh.nextHalfedge(m_mesh.nextHalfedge(h0)));

        auto h1 = m_mesh.nextHalfedge(h0);
        m_mesh.insertVertex(m_mesh.edge(h1), fpoint[f]);

        auto h =
            m_mesh.nextHalfedge(m_mesh.nextHalfedge(m_mesh.nextHalfedge(h1)));
        while (h != h0)
        {
            m_mesh.insertEdge(h1, h);
            h = m_mesh.nextHalfedge(
                m_mesh.nextHalfedge(m_mesh.nextHalfedge(h1)));
        }
    }

    // clean-up properties
    m_mesh.removeVertexProperty(vpoint);
    m_mesh.removeEdgeProperty(epoint);
    m_mesh.removeFaceProperty(fpoint);
}

//-----------------------------------------------------------------------------

void SurfaceSubdivision::loop()
{
    if (!m_mesh.isTriangleMesh())
        return;

    // reserve memory
    size_t nv = m_mesh.nVertices();
    size_t ne = m_mesh.nEdges();
    size_t nf = m_mesh.nFaces();
    m_mesh.reserve(nv + ne, 2 * ne + 3 * nf, 4 * nf);

    // add properties
    auto vpoint = m_mesh.addVertexProperty<Point>("loop:vpoint");
    auto epoint = m_mesh.addEdgeProperty<Point>("loop:epoint");

    // compute vertex positions
    for (auto v : m_mesh.vertices())
    {
        // isolated vertex?
        if (m_mesh.isIsolated(v))
        {
            vpoint[v] = m_points[v];
        }

        // boundary vertex?
        else if (m_mesh.isSurfaceBoundary(v))
        {
            auto h1 = m_mesh.halfedge(v);
            auto h0 = m_mesh.prevHalfedge(h1);

            Point p = m_points[v];
            p *= 6.0;
            p += m_points[m_mesh.toVertex(h1)];
            p += m_points[m_mesh.fromVertex(h0)];
            p *= 0.125;

            vpoint[v] = p;
        }

        // interior feature vertex?
        else if (m_vfeature && m_vfeature[v])
        {
            Point p = m_points[v];
            p *= 6.0;
            int count(0);

            for (auto h : m_mesh.halfedges(v))
            {
                if (m_efeature[m_mesh.edge(h)])
                {
                    p += m_points[m_mesh.toVertex(h)];
                    ++count;
                }
            }

            if (count == 2) // vertex is on feature edge
            {
                p *= 0.125;
                vpoint[v] = p;
            }
            else // keep fixed
            {
                vpoint[v] = m_points[v];
            }
        }

        // interior vertex
        else
        {
            Point  p(0, 0, 0);
            Scalar k(0);

            for (auto vv : m_mesh.vertices(v))
            {
                p += m_points[vv];
                ++k;
            }
            p /= k;

            Scalar beta =
                (0.625 - pow(0.375 + 0.25 * cos(2.0 * M_PI / k), 2.0));

            vpoint[v] = m_points[v] * (Scalar)(1.0 - beta) + beta * p;
        }
    }

    // compute edge positions
    for (auto e : m_mesh.edges())
    {
        // boundary or feature edge?
        if (m_mesh.isSurfaceBoundary(e) || (m_efeature && m_efeature[e]))
        {
            epoint[e] = (m_points[m_mesh.vertex(e, 0)] +
                         m_points[m_mesh.vertex(e, 1)]) *
                        Scalar(0.5);
        }

        // interior edge
        else
        {
            auto  h0 = m_mesh.halfedge(e, 0);
            auto  h1 = m_mesh.halfedge(e, 1);
            Point p  = m_points[m_mesh.toVertex(h0)];
            p += m_points[m_mesh.toVertex(h1)];
            p *= 3.0;
            p += m_points[m_mesh.toVertex(m_mesh.nextHalfedge(h0))];
            p += m_points[m_mesh.toVertex(m_mesh.nextHalfedge(h1))];
            p *= 0.125;
            epoint[e] = p;
        }
    }

    // set new vertex positions
    for (auto v : m_mesh.vertices())
    {
        m_points[v] = vpoint[v];
    }

    // insert new vertices on edges
    for (auto e : m_mesh.edges())
    {
        // feature edge?
        if (m_efeature && m_efeature[e])
        {
            auto h  = m_mesh.insertVertex(e, epoint[e]);
            auto v  = m_mesh.toVertex(h);
            auto e0 = m_mesh.edge(h);
            auto e1 = m_mesh.edge(m_mesh.nextHalfedge(h));

            m_vfeature[v]  = true;
            m_efeature[e0] = true;
            m_efeature[e1] = true;
        }

        // normal edge
        else
        {
            m_mesh.insertVertex(e, epoint[e]);
        }
    }

    // split faces
    SurfaceMesh::Halfedge h;
    for (auto f : m_mesh.faces())
    {
        h = m_mesh.halfedge(f);
        m_mesh.insertEdge(h, m_mesh.nextHalfedge(m_mesh.nextHalfedge(h)));
        h = m_mesh.nextHalfedge(h);
        m_mesh.insertEdge(h, m_mesh.nextHalfedge(m_mesh.nextHalfedge(h)));
        h = m_mesh.nextHalfedge(h);
        m_mesh.insertEdge(h, m_mesh.nextHalfedge(m_mesh.nextHalfedge(h)));
    }

    // clean-up properties
    m_mesh.removeVertexProperty(vpoint);
    m_mesh.removeEdgeProperty(epoint);
}

//-----------------------------------------------------------------------------

void SurfaceSubdivision::sqrt3()
{
    // reserve memory
    int nv = m_mesh.nVertices();
    int ne = m_mesh.nEdges();
    int nf = m_mesh.nFaces();
    m_mesh.reserve(nv + nf, ne + 3 * nf, 3 * nf);

    auto points = m_mesh.vertexProperty<Point>("v:point");

    // remember end of old vertices and edges
    auto vend = m_mesh.verticesEnd();
    auto eend = m_mesh.edgesEnd();

    // compute new positions of old vertices
    auto new_pos = m_mesh.addVertexProperty<Point>("v:np");
    for (auto v : m_mesh.vertices())
    {
        if (!m_mesh.isSurfaceBoundary(v))
        {
            Scalar n     = m_mesh.valence(v);
            Scalar alpha = (4.0 - 2.0 * cos(2.0 * M_PI / n)) / 9.0;
            Point  p(0, 0, 0);

            for (auto vv : m_mesh.vertices(v))
                p += m_points[vv];

            p          = (1.0f - alpha) * m_points[v] + alpha / n * p;
            new_pos[v] = p;
        }
    }

    // split faces
    for (auto f : m_mesh.faces())
    {
        Point  p(0, 0, 0);
        Scalar c(0);

        for (auto fv : m_mesh.vertices(f))
        {
            p += m_points[fv];
            ++c;
        }

        p /= c;

        m_mesh.split(f, p);
    }

    // set new positions of old vertices
    for (auto vit = m_mesh.verticesBegin(); vit != vend; ++vit)
    {
        if (!m_mesh.isSurfaceBoundary(*vit))
        {
            points[*vit] = new_pos[*vit];
        }
    }

    m_mesh.removeVertexProperty(new_pos);

    // flip old edges
    for (auto eit = m_mesh.edgesBegin(); eit != eend; ++eit)
    {
        if (m_mesh.isFlipOk(*eit))
        {
            m_mesh.flip(*eit);
        }
    }
}

//=============================================================================
} // namespace pmp
//=============================================================================
