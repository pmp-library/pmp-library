// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/algorithms/features.h"
#include "pmp/algorithms/normals.h"

namespace pmp {

size_t detect_features(SurfaceMesh& mesh, Scalar angle)
{
    auto vfeature = mesh.vertex_property("v:feature", false);
    auto efeature = mesh.edge_property("e:feature", false);
    const Scalar feature_cosine = cos(angle / 180.0 * M_PI);
    size_t n_edges = 0;
    for (auto e : mesh.edges())
    {
        if (!mesh.is_boundary(e))
        {
            const auto f0 = mesh.face(mesh.halfedge(e, 0));
            const auto f1 = mesh.face(mesh.halfedge(e, 1));

            const Normal n0 = face_normal(mesh, f0);
            const Normal n1 = face_normal(mesh, f1);

            if (dot(n0, n1) < feature_cosine)
            {
                efeature[e] = true;
                vfeature[mesh.vertex(e, 0)] = true;
                vfeature[mesh.vertex(e, 1)] = true;
                n_edges++;
            }
        }
    }
    return n_edges;
}

size_t detect_boundary(SurfaceMesh& mesh)
{
    auto vfeature = mesh.vertex_property("v:feature", false);
    auto efeature = mesh.edge_property("e:feature", false);
    for (auto v : mesh.vertices())
        if (mesh.is_boundary(v))
            vfeature[v] = true;

    size_t n_edges = 0;
    for (auto e : mesh.edges())
        if (mesh.is_boundary(e))
        {
            efeature[e] = true;
            n_edges++;
        }
    return n_edges;
}

void clear_features(SurfaceMesh& mesh)
{
    auto vfeature = mesh.vertex_property("v:feature", false);
    auto efeature = mesh.edge_property("e:feature", false);
    for (auto v : mesh.vertices())
        vfeature[v] = false;

    for (auto e : mesh.edges())
        efeature[e] = false;
}

} // namespace pmp
