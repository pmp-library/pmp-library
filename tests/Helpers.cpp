// Copyright 2021 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "Helpers.h"

#include "pmp/algorithms/SurfaceFactory.h"
#include "pmp/algorithms/SurfaceRemeshing.h"

namespace pmp {

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

SurfaceMesh hemisphere()
{
    // generate quad sphere mesh and triangulate it
    auto mesh = SurfaceFactory::quad_sphere(5);
    mesh.triangulate();

    // delete lower half
    for (auto v : mesh.vertices())
        if (mesh.position(v)[1] < -0.01)
            mesh.delete_vertex(v);
    mesh.garbage_collection();

    // remesh to get nice but irregular triangulation
    SurfaceRemeshing(mesh).uniform_remeshing(0.05);

    return mesh;
}

} // namespace pmp