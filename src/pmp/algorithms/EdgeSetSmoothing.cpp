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

#include "EdgeSetSmoothing.h"

//=============================================================================

namespace pmp {

//=============================================================================

EdgeSetSmoothing::EdgeSetSmoothing(EdgeSet& edgeSet) : m_edgeSet(edgeSet)
{
}

//-----------------------------------------------------------------------------

void EdgeSetSmoothing::smooth(unsigned int nIterations)
{
    // get/add properties
    auto points = m_edgeSet.getVertexProperty<Point>("v:point");
    auto newPoint = m_edgeSet.addVertexProperty<Point>("v:newPoint");

    for (unsigned int i = 0; i < nIterations; ++i)
    {
        // compute new vertex positions
        for (auto v : m_edgeSet.vertices())
        {
            if (!m_edgeSet.isIsolated(v))
            {
                Scalar s(0.0);
                Point p(0.0);

                for (auto vv : m_edgeSet.vertices(v))
                {
                    p += points[vv];
                    s += 1.0;
                }

                newPoint[v] = p / s;
            }
        }

        // update vertex positions
        for (auto v : m_edgeSet.vertices())
        {
            points[v] = newPoint[v];
        }
    }

    // remove edge property
    m_edgeSet.removeVertexProperty(newPoint);
}

//=============================================================================
} // namespace pmp
//=============================================================================
