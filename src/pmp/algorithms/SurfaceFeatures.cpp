// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/algorithms/SurfaceFeatures.h"
#include "pmp/algorithms/SurfaceNormals.h"

namespace pmp {

SurfaceFeatures::SurfaceFeatures(SurfaceMesh& mesh) : mesh_(mesh)
{
    vfeature_ = mesh_.vertex_property("v:feature", false);
    efeature_ = mesh_.edge_property("e:feature", false);
}

void SurfaceFeatures::clear()
{
    for (auto v : mesh_.vertices())
        vfeature_[v] = false;

    for (auto e : mesh_.edges())
        efeature_[e] = false;
}

void SurfaceFeatures::detect_boundary()
{
    for (auto v : mesh_.vertices())
        if (mesh_.is_boundary(v))
            vfeature_[v] = true;

    for (auto e : mesh_.edges())
        if (mesh_.is_boundary(e))
            efeature_[e] = true;
}

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

} // namespace pmp
