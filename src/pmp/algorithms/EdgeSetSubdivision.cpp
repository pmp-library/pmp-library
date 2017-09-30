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

#include "EdgeSetSubdivision.h"

//=============================================================================

namespace pmp {

//=============================================================================

EdgeSetSubdivision::EdgeSetSubdivision(EdgeSet& edgeSet) : m_edgeSet(edgeSet)
{
}

//-----------------------------------------------------------------------------

void EdgeSetSubdivision::subdivide()
{
    // get/add properties
    auto points   = m_edgeSet.getVertexProperty<Point>("v:point");
    auto newPoint = m_edgeSet.addEdgeProperty<Point>("v:newPoint");

    EdgeSet::Vertex   v0, v1, v2, v3;
    EdgeSet::Halfedge h;

    for (auto e : m_edgeSet.edges())
    {
        h = m_edgeSet.halfedge(e, 0);

        v0 = m_edgeSet.fromVertex(m_edgeSet.prevHalfedge(h));
        v1 = m_edgeSet.fromVertex(h);
        v2 = m_edgeSet.toVertex(h);
        v3 = m_edgeSet.toVertex(m_edgeSet.nextHalfedge(h));

        newPoint[e] = 0.0625f * (9.0f * (points[v1] + points[v2]) - points[v0] -
                                 points[v3]);
    }

    // insert edge vertices
    for (auto e : m_edgeSet.edges())
    {
        m_edgeSet.insertVertex(e, newPoint[e]);
    }

    // remove edge property
    m_edgeSet.removeEdgeProperty(newPoint);
}

//=============================================================================
} // namespace pmp
//=============================================================================
