// Copyright 2022 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/algorithms/utilities.h"
#include "pmp/algorithms/differential_geometry.h"
#include <limits>

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

Scalar min_face_area(const SurfaceMesh& mesh)
{
    auto min_area = std::numeric_limits<Scalar>::max();
    for (auto f : mesh.faces())
    {
        auto area = face_area(mesh, f);
        min_area = std::min(area, min_area);
    }
    return min_area;
}

Scalar mean_edge_length(const SurfaceMesh& mesh)
{
    Scalar length{0};
    for (auto e : mesh.edges())
        length += edge_length(mesh, e);
    length /= (Scalar)mesh.n_edges();
    return length;
}

} // namespace pmp