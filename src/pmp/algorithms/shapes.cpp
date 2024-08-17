// Copyright 2011-2021 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/algorithms/shapes.h"
#include "pmp/algorithms/subdivision.h"
#include "pmp/algorithms/differential_geometry.h"

#include <cmath>
#include <algorithm>
#include <numbers>

namespace pmp {
namespace {
void project_to_unit_sphere(SurfaceMesh& mesh)
{
    for (auto v : mesh.vertices())
    {
        auto p = mesh.position(v);
        auto n = norm(p);
        mesh.position(v) = (1.0 / n) * p;
    }
}
} // namespace

SurfaceMesh tetrahedron()
{
    SurfaceMesh mesh;
    float a = 1.0f / 3.0f;
    float b = sqrt(8.0f / 9.0f);
    float c = sqrt(2.0f / 9.0f);
    float d = sqrt(2.0f / 3.0f);

    auto v0 = mesh.add_vertex(Point(0, 0, 1));
    auto v1 = mesh.add_vertex(Point(-c, d, -a));
    auto v2 = mesh.add_vertex(Point(-c, -d, -a));
    auto v3 = mesh.add_vertex(Point(b, 0, -a));

    mesh.add_triangle(v0, v1, v2);
    mesh.add_triangle(v0, v2, v3);
    mesh.add_triangle(v0, v3, v1);
    mesh.add_triangle(v3, v2, v1);

    return mesh;
}

SurfaceMesh hexahedron()
{
    SurfaceMesh mesh;

    float a = 1.0f / sqrt(3.0f);
    auto v0 = mesh.add_vertex(Point(-a, -a, -a));
    auto v1 = mesh.add_vertex(Point(a, -a, -a));
    auto v2 = mesh.add_vertex(Point(a, a, -a));
    auto v3 = mesh.add_vertex(Point(-a, a, -a));
    auto v4 = mesh.add_vertex(Point(-a, -a, a));
    auto v5 = mesh.add_vertex(Point(a, -a, a));
    auto v6 = mesh.add_vertex(Point(a, a, a));
    auto v7 = mesh.add_vertex(Point(-a, a, a));

    mesh.add_quad(v3, v2, v1, v0);
    mesh.add_quad(v2, v6, v5, v1);
    mesh.add_quad(v5, v6, v7, v4);
    mesh.add_quad(v0, v4, v7, v3);
    mesh.add_quad(v3, v7, v6, v2);
    mesh.add_quad(v1, v5, v4, v0);

    return mesh;
}

SurfaceMesh octahedron()
{
    auto mesh = hexahedron();
    dual(mesh);
    project_to_unit_sphere(mesh);
    return mesh;
}

SurfaceMesh dodecahedron()
{
    auto mesh = icosahedron();
    dual(mesh);
    project_to_unit_sphere(mesh);
    return mesh;
}

SurfaceMesh icosahedron()
{
    SurfaceMesh mesh;

    float phi = (1.0f + sqrt(5.0f)) * 0.5f; // golden ratio
    float a = 1.0f;
    float b = 1.0f / phi;

    auto v1 = mesh.add_vertex(Point(0, b, -a));
    auto v2 = mesh.add_vertex(Point(b, a, 0));
    auto v3 = mesh.add_vertex(Point(-b, a, 0));
    auto v4 = mesh.add_vertex(Point(0, b, a));
    auto v5 = mesh.add_vertex(Point(0, -b, a));
    auto v6 = mesh.add_vertex(Point(-a, 0, b));
    auto v7 = mesh.add_vertex(Point(0, -b, -a));
    auto v8 = mesh.add_vertex(Point(a, 0, -b));
    auto v9 = mesh.add_vertex(Point(a, 0, b));
    auto v10 = mesh.add_vertex(Point(-a, 0, -b));
    auto v11 = mesh.add_vertex(Point(b, -a, 0));
    auto v12 = mesh.add_vertex(Point(-b, -a, 0));

    project_to_unit_sphere(mesh);

    mesh.add_triangle(v3, v2, v1);
    mesh.add_triangle(v2, v3, v4);
    mesh.add_triangle(v6, v5, v4);
    mesh.add_triangle(v5, v9, v4);
    mesh.add_triangle(v8, v7, v1);
    mesh.add_triangle(v7, v10, v1);
    mesh.add_triangle(v12, v11, v5);
    mesh.add_triangle(v11, v12, v7);
    mesh.add_triangle(v10, v6, v3);
    mesh.add_triangle(v6, v10, v12);
    mesh.add_triangle(v9, v8, v2);
    mesh.add_triangle(v8, v9, v11);
    mesh.add_triangle(v3, v6, v4);
    mesh.add_triangle(v9, v2, v4);
    mesh.add_triangle(v10, v3, v1);
    mesh.add_triangle(v2, v8, v1);
    mesh.add_triangle(v12, v10, v7);
    mesh.add_triangle(v8, v11, v7);
    mesh.add_triangle(v6, v12, v5);
    mesh.add_triangle(v11, v9, v5);

    return mesh;
}

SurfaceMesh icosphere(size_t n_subdivisions)
{
    auto mesh = icosahedron();
    for (size_t i = 0; i < n_subdivisions; i++)
    {
        loop_subdivision(mesh);
        project_to_unit_sphere(mesh);
    }
    return mesh;
}

SurfaceMesh quad_sphere(size_t n_subdivisions)
{
    auto mesh = hexahedron();

    for (size_t i = 0; i < n_subdivisions; i++)
    {
        catmull_clark_subdivision(mesh);
        project_to_unit_sphere(mesh);
    }
    return mesh;
}

SurfaceMesh uv_sphere(const Point& center, Scalar radius, size_t n_slices,
                      size_t n_stacks)
{
    SurfaceMesh mesh;

    // add top vertex
    const auto top = Point(center[0], center[1] + radius, center[2]);
    const auto v0 = mesh.add_vertex(top);

    // generate vertices per stack / slice
    for (size_t i = 0; i < n_stacks - 1; i++)
    {
        const auto phi = std::numbers::pi * double(i + 1) / double(n_stacks);
        for (size_t j = 0; j < n_slices; ++j)
        {
            const auto theta =
                2.0 * std::numbers::pi * double(j) / double(n_slices);
            const auto x = center[0] + radius * std::sin(phi) * std::cos(theta);
            const auto y = center[1] + radius * std::cos(phi);
            const auto z = center[2] + radius * std::sin(phi) * std::sin(theta);
            mesh.add_vertex(Point(x, y, z));
        }
    }

    // add bottom vertex
    const auto bottom = Point(center[0], center[1] - radius, center[2]);
    const auto v1 = mesh.add_vertex(bottom);

    // add top / bottom triangles
    for (size_t i = 0; i < n_slices; ++i)
    {
        const auto i0 = i + 1;
        const auto i1 = (i + 1) % n_slices + 1;
        mesh.add_triangle(v0, Vertex(i1), Vertex(i0));

        const auto i2 = i + n_slices * (n_stacks - 2) + 1;
        const auto i3 = (i + 1) % n_slices + n_slices * (n_stacks - 2) + 1;
        mesh.add_triangle(v1, Vertex(i2), Vertex(i3));
    }

    // add quads per stack / slice
    for (size_t j = 0; j < n_stacks - 2; ++j)
    {
        const auto idx0 = j * n_slices + 1;
        const auto idx1 = (j + 1) * n_slices + 1;
        for (size_t i = 0; i < n_slices; ++i)
        {
            const auto i0 = idx0 + i;
            const auto i1 = idx0 + (i + 1) % n_slices;
            const auto i2 = idx1 + (i + 1) % n_slices;
            const auto i3 = idx1 + i;
            mesh.add_quad(Vertex(i0), Vertex(i1), Vertex(i2), Vertex(i3));
        }
    }

    return mesh;
}

SurfaceMesh plane(size_t resolution)
{
    assert(resolution >= 1);

    SurfaceMesh mesh;

    // generate vertices
    Point p(0, 0, 0);
    for (size_t i = 0; i < resolution + 1; i++)
    {
        for (size_t j = 0; j < resolution + 1; j++)
        {
            mesh.add_vertex(p);
            p[1] += 1.0 / resolution;
        }
        p[1] = 0;
        p[0] += 1.0 / resolution;
    }

    // generate faces
    for (size_t i = 0; i < resolution; i++)
    {
        for (size_t j = 0; j < resolution; j++)
        {
            auto v0 = Vertex(j + i * (resolution + 1));
            auto v1 = Vertex(v0.idx() + resolution + 1);
            auto v2 = Vertex(v0.idx() + resolution + 2);
            auto v3 = Vertex(v0.idx() + 1);
            mesh.add_quad(v0, v1, v2, v3);
        }
    }

    return mesh;
}

SurfaceMesh cone(size_t n_subdivisions, Scalar radius, Scalar height)
{
    assert(n_subdivisions >= 3);

    SurfaceMesh mesh;

    // add vertices subdividing a circle
    std::vector<Vertex> base_vertices;
    for (size_t i = 0; i < n_subdivisions; i++)
    {
        Scalar ratio = static_cast<Scalar>(i) / (n_subdivisions);
        Scalar r = ratio * (std::numbers::pi * 2.0);
        Scalar x = std::cos(r) * radius;
        Scalar y = std::sin(r) * radius;
        auto v = mesh.add_vertex(Point(x, y, 0.0));
        base_vertices.push_back(v);
    }

    // add the tip of the cone
    auto v0 = mesh.add_vertex(Point(0.0, 0.0, height));

    // generate triangular faces
    for (size_t i = 0; i < n_subdivisions; i++)
    {
        auto ii = (i + 1) % n_subdivisions;
        mesh.add_triangle(v0, Vertex(i), Vertex(ii));
    }

    // reverse order for consistent face orientation
    std::reverse(base_vertices.begin(), base_vertices.end());

    // add polygonal base face
    mesh.add_face(base_vertices);

    return mesh;
}

SurfaceMesh cylinder(size_t n_subdivisions, Scalar radius, Scalar height)
{
    assert(n_subdivisions >= 3);

    SurfaceMesh mesh;

    // generate vertices
    std::vector<Vertex> bottom_vertices;
    std::vector<Vertex> top_vertices;
    for (size_t i = 0; i < n_subdivisions; i++)
    {
        Scalar ratio = static_cast<Scalar>(i) / (n_subdivisions);
        Scalar r = ratio * (std::numbers::pi * 2.0);
        Scalar x = std::cos(r) * radius;
        Scalar y = std::sin(r) * radius;
        Vertex v = mesh.add_vertex(Point(x, y, 0.0));
        bottom_vertices.push_back(v);
        v = mesh.add_vertex(Point(x, y, height));
        top_vertices.push_back(v);
    }

    // add faces around the cylinder
    for (size_t i = 0; i < n_subdivisions; i++)
    {
        auto ii = i * 2;
        auto jj = (ii + 2) % (n_subdivisions * 2);
        auto kk = (ii + 3) % (n_subdivisions * 2);
        auto ll = ii + 1;
        mesh.add_quad(Vertex(ii), Vertex(jj), Vertex(kk), Vertex(ll));
    }

    // add top polygon
    mesh.add_face(top_vertices);

    // reverse order for consistent face orientation
    std::reverse(bottom_vertices.begin(), bottom_vertices.end());

    // add bottom polygon
    mesh.add_face(bottom_vertices);

    return mesh;
}

SurfaceMesh torus(size_t radial_resolution, size_t tubular_resolution,
                  Scalar radius, Scalar thickness)
{
    assert(radial_resolution >= 3);
    assert(tubular_resolution >= 3);

    SurfaceMesh mesh;

    // generate vertices
    for (size_t i = 0; i < radial_resolution; i++)
    {
        for (size_t j = 0; j < tubular_resolution; j++)
        {
            Scalar u = static_cast<Scalar>(j) / tubular_resolution *
                       std::numbers::pi * 2.0;
            Scalar v = static_cast<Scalar>(i) / radial_resolution *
                       std::numbers::pi * 2.0;
            Scalar x = (radius + thickness * std::cos(v)) * std::cos(u);
            Scalar y = (radius + thickness * std::cos(v)) * std::sin(u);
            Scalar z = thickness * std::sin(v);
            mesh.add_vertex(Point(x, y, z));
        }
    }

    // add quad faces
    for (size_t i = 0; i < radial_resolution; i++)
    {
        auto i_next = (i + 1) % radial_resolution;
        for (size_t j = 0; j < tubular_resolution; j++)
        {
            auto j_next = (j + 1) % tubular_resolution;
            auto i0 = i * tubular_resolution + j;
            auto i1 = i * tubular_resolution + j_next;
            auto i2 = i_next * tubular_resolution + j_next;
            auto i3 = i_next * tubular_resolution + j;
            mesh.add_quad(Vertex(i0), Vertex(i1), Vertex(i2), Vertex(i3));
        }
    }

    return mesh;
}

} // namespace pmp