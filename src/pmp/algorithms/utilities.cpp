// Copyright 2022 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#include "pmp/algorithms/utilities.h"
#include "pmp/algorithms/differential_geometry.h"
#include <algorithm>
#include <limits>
#include <queue>

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
        std::ranges::reverse(vertices);
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

Scalar min_edge_length(const SurfaceMesh& mesh)
{
    Scalar min_length = std::numeric_limits<Scalar>::max();
    for (auto e : mesh.edges())
        min_length = std::min(min_length, edge_length(mesh, e));
    return min_length;
}

int connected_components(SurfaceMesh& mesh)
{
    auto component = mesh.vertex_property<int>("v:component");

    // make sure to initialize all vertices with -1
    for (auto v : mesh.vertices())
        component[v] = -1;

    int idx = 0;
    for (auto v : mesh.vertices())
    {
        if (component[v] != -1)
            continue;

        std::queue<Vertex> vertices;
        component[v] = idx;
        vertices.push(v);

        while (!vertices.empty())
        {
            auto vv = vertices.front();
            vertices.pop();

            for (auto vc : mesh.vertices(vv))
            {
                if (component[vc] == -1)
                {
                    component[vc] = idx;
                    vertices.push(vc);
                }
            }
        }
        idx++;
    }

    return idx;
}

} // namespace pmp