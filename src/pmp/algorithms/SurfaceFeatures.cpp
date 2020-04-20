//=============================================================================
// Copyright (C) 2011-2019 The pmp-library developers
//
// This file is part of the Polygon Mesh Processing Library.
// Distributed under a MIT-style license, see LICENSE.txt for details.
//
// SPDX-License-Identifier: MIT-with-employer-disclaimer
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

size_t SurfaceFeatures::detect_boundary()
{
    for (auto v : mesh_.vertices())
        if (mesh_.is_boundary(v))
            vfeature_[v] = true;

    size_t n_edges = 0;
    for (auto e : mesh_.edges())
        if (mesh_.is_boundary(e))
        {
            efeature_[e] = true;
            n_edges++;
        }
    return n_edges;
}

//-----------------------------------------------------------------------------

size_t SurfaceFeatures::detect_angle(Scalar angle)
{
    const Scalar feature_cosine = cos(angle / 180.0 * M_PI);
    size_t n_edges = 0;
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
                n_edges++;
            }
        }
    }
    return n_edges;
}

//=============================================================================
} // namespace pmp
//=============================================================================
