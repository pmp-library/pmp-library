// Copyright 2021 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "Helpers.h"

#include "pmp/algorithms/SurfaceFactory.h"
#include "pmp/algorithms/SurfaceRemeshing.h"
#include "pmp/algorithms/SurfaceFeatures.h"
#include "pmp/algorithms/SurfaceSubdivision.h"
#include "pmp/algorithms/SurfaceTriangulation.h"

namespace pmp {

static SurfaceMesh hemisphere_mesh;
static SurfaceMesh icosahedron_mesh;

SurfaceMesh vertex_onering()
{
    SurfaceMesh mesh;

    auto v0 = mesh.add_vertex(Point(0.4499998093, 0.5196152329, 0.0000000000));
    auto v1 = mesh.add_vertex(Point(0.2999998033, 0.5196152329, 0.0000000000));
    auto v2 = mesh.add_vertex(Point(0.5249998569, 0.3897114396, 0.0000000000));
    auto v3 = mesh.add_vertex(Point(0.3749998510, 0.3897114396, 0.0000000000));
    auto v4 = mesh.add_vertex(Point(0.2249998450, 0.3897114396, 0.0000000000));
    auto v5 = mesh.add_vertex(Point(0.4499999285, 0.2598076165, 0.0000000000));
    auto v6 = mesh.add_vertex(Point(0.2999999225, 0.2598076165, 0.0000000000));

    mesh.add_triangle(v3, v0, v1);
    mesh.add_triangle(v3, v2, v0);
    mesh.add_triangle(v4, v3, v1);
    mesh.add_triangle(v5, v2, v3);
    mesh.add_triangle(v6, v5, v3);
    mesh.add_triangle(v6, v3, v4);

    return mesh;
}

SurfaceMesh edge_onering()
{
    SurfaceMesh mesh;

    auto v0 = mesh.add_vertex(Point(0.5999997854, 0.5196152329, 0.0000000000));
    auto v1 = mesh.add_vertex(Point(0.4499998093, 0.5196152329, 0.0000000000));
    auto v2 = mesh.add_vertex(Point(0.2999998033, 0.5196152329, 0.0000000000));
    auto v3 = mesh.add_vertex(Point(0.6749998331, 0.3897114396, 0.0000000000));
    auto v4 = mesh.add_vertex(Point(0.5249998569, 0.3897114396, 0.0000000000));
    auto v5 = mesh.add_vertex(Point(0.3749998510, 0.3897114396, 0.0000000000));
    auto v6 = mesh.add_vertex(Point(0.2249998450, 0.3897114396, 0.0000000000));
    auto v7 = mesh.add_vertex(Point(0.5999999046, 0.2598076165, 0.0000000000));
    auto v8 = mesh.add_vertex(Point(0.4499999285, 0.2598076165, 0.0000000000));
    auto v9 = mesh.add_vertex(Point(0.2999999225, 0.2598076165, 0.0000000000));

    mesh.add_triangle(v4, v0, v1);
    mesh.add_triangle(v4, v3, v0);
    mesh.add_triangle(v5, v1, v2);
    mesh.add_triangle(v5, v4, v1);
    mesh.add_triangle(v6, v5, v2);
    mesh.add_triangle(v7, v3, v4);
    mesh.add_triangle(v8, v7, v4);
    mesh.add_triangle(v8, v4, v5);
    mesh.add_triangle(v9, v8, v5);
    mesh.add_triangle(v9, v5, v6);

    return mesh;
}

SurfaceMesh hemisphere()
{
    if (hemisphere_mesh.is_empty())
    {
        // use ref for brevity
        auto& mesh = hemisphere_mesh;

        // generate quad sphere mesh and triangulate it
        mesh = SurfaceFactory::quad_sphere(3);
        SurfaceTriangulation tr(mesh);
        tr.triangulate();

        // delete lower half
        for (auto v : mesh.vertices())
            if (mesh.position(v)[1] < -0.01)
                mesh.delete_vertex(v);
        mesh.garbage_collection();

        // remesh to get nice but irregular triangulation
        SurfaceRemeshing(mesh).uniform_remeshing(0.1);
    }
    return hemisphere_mesh;
}

SurfaceMesh subdivided_icosahedron()
{
    if (icosahedron_mesh.is_empty())
    {
        // use ref for brevity
        auto& mesh = icosahedron_mesh;
        mesh = SurfaceFactory::icosahedron();

        // select all edges as features
        SurfaceFeatures sf(mesh);
        sf.detect_angle(25);

        // feature-preserving subdivision
        SurfaceSubdivision subdiv(mesh);
        subdiv.loop();
        subdiv.loop();
        subdiv.loop();
    }
    return icosahedron_mesh;
}

SurfaceMesh l_shape()
{
    SurfaceMesh mesh;

    std::vector<Vertex> vertices;

    vertices.push_back(mesh.add_vertex(Point(0.0, 0.0, 0.0)));
    vertices.push_back(mesh.add_vertex(Point(0.5, 0.0, 0.0)));
    vertices.push_back(mesh.add_vertex(Point(1.0, 0.0, 0.0)));
    vertices.push_back(mesh.add_vertex(Point(1.0, 0.5, 0.0)));
    vertices.push_back(mesh.add_vertex(Point(0.5, 0.5, 0.0)));
    vertices.push_back(mesh.add_vertex(Point(0.5, 1.0, 0.0)));
    vertices.push_back(mesh.add_vertex(Point(0.5, 1.5, 0.0)));
    vertices.push_back(mesh.add_vertex(Point(0.5, 2.0, 0.0)));
    vertices.push_back(mesh.add_vertex(Point(0.0, 2.0, 0.0)));
    vertices.push_back(mesh.add_vertex(Point(0.0, 1.5, 0.0)));
    vertices.push_back(mesh.add_vertex(Point(0.0, 1.0, 0.0)));
    vertices.push_back(mesh.add_vertex(Point(0.0, 0.5, 0.0)));

    mesh.add_face(vertices);

    return mesh;
}

} // namespace pmp