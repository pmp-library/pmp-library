// Copyright 2022 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/utilities.h"

namespace pmp {

BoundingBox bounds(const SurfaceMesh& mesh)
{
    BoundingBox bb;
    for (auto v : mesh.vertices())
        bb += mesh.position(v);
    return bb;
}

void flip_faces(SurfaceMesh& mesh)
{
    SurfaceMesh new_mesh;
    for (auto v : mesh.vertices())
    {
        new_mesh.add_vertex(mesh.position(v));
    }
    for (auto f : mesh.faces())
    {
        std::vector<Vertex> vertices;
        for (auto v : mesh.vertices(f))
        {
            vertices.push_back(v);
        }
        std::reverse(vertices.begin(), vertices.end());
        new_mesh.add_face(vertices);
    }
    mesh = new_mesh;
}

} // namespace pmp