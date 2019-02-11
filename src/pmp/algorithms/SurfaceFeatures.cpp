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

#include <pmp/algorithms/SurfaceFeatures.h>
#include <pmp/algorithms/SurfaceNormals.h>

//=============================================================================

namespace pmp {

//=============================================================================

SurfaceFeatures::SurfaceFeatures(SurfaceMesh& mesh) : mesh_(mesh)
{
    vfeature_ = mesh_.vertex_property("v:feature", false);
    efeature_ = mesh_.edge_property("e:feature", false);
}

//-----------------------------------------------------------------------------

void SurfaceFeatures::clear()
{
    for (auto v : mesh_.vertices())
        vfeature_[v] = false;

    for (auto e : mesh_.edges())
        efeature_[e] = false;
}

//-----------------------------------------------------------------------------

void SurfaceFeatures::detect_boundary()
{
    for (auto v : mesh_.vertices())
        if (mesh_.is_boundary(v))
            vfeature_[v] = true;

    for (auto e : mesh_.edges())
        if (mesh_.is_boundary(e))
            efeature_[e] = true;
}

//-----------------------------------------------------------------------------

void SurfaceFeatures::detect_angle(Scalar angle)
{
    const Scalar feature_cosine = cos(angle / 180.0 * M_PI);

    for (auto e : mesh_.edges())
    {
        if (!mesh_.is_boundary(e))
        {
            const auto f0 = mesh_.face(mesh_.halfedge(e, 0));
            const auto f1 = mesh_.face(mesh_.halfedge(e, 1));

            const Normal n0 = SurfaceNormals::compute_face_normal(mesh_, f0);
            const Normal n1 = SurfaceNormals::compute_face_normal(mesh_, f1);

            if (dot(n0, n1) < feature_cosine)
            {
                efeature_[e] = true;
                vfeature_[mesh_.vertex(e, 0)] = true;
                vfeature_[mesh_.vertex(e, 1)] = true;
            }
        }
    }
}

//=============================================================================
} // namespace pmp
//=============================================================================
