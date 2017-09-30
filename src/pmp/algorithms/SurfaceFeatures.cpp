//=============================================================================
// Copyright (C) 2011-2016 by Graphics & Geometry Group, Bielefeld University
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

#include "SurfaceFeatures.h"

//=============================================================================

namespace pmp {

//=============================================================================

SurfaceFeatures::SurfaceFeatures(SurfaceMesh& mesh) : m_mesh(mesh)
{
    m_vfeature = m_mesh.vertexProperty("v:feature", false);
    m_efeature = m_mesh.edgeProperty("e:feature", false);
}

//-----------------------------------------------------------------------------

void SurfaceFeatures::clear()
{
    for (auto v : m_mesh.vertices())
        m_vfeature[v] = false;

    for (auto e : m_mesh.edges())
        m_efeature[e] = false;
}

//-----------------------------------------------------------------------------

void SurfaceFeatures::detectBoundary()
{
    for (auto v : m_mesh.vertices())
        if (m_mesh.isBoundary(v))
            m_vfeature[v] = true;

    for (auto e : m_mesh.edges())
        if (m_mesh.isBoundary(e))
            m_efeature[e] = true;
}

//-----------------------------------------------------------------------------

void SurfaceFeatures::detectAngle(Scalar angle)
{
    const Scalar feature_cosine = cos(angle / 180.0 * M_PI);

    for (auto e : m_mesh.edges())
    {
        if (!m_mesh.isBoundary(e))
        {
            const auto f0 = m_mesh.face(m_mesh.halfedge(e, 0));
            const auto f1 = m_mesh.face(m_mesh.halfedge(e, 1));

            const Normal n0 = m_mesh.computeFaceNormal(f0);
            const Normal n1 = m_mesh.computeFaceNormal(f1);

            if (dot(n0, n1) < feature_cosine)
            {
                m_efeature[e] = true;
                m_vfeature[m_mesh.vertex(e, 0)] = true;
                m_vfeature[m_mesh.vertex(e, 1)] = true;
            }
        }
    }
}

//=============================================================================
} // namespace pmp
//=============================================================================
