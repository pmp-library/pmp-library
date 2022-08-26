// Copyright 2021 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "Helpers.h"

#include "pmp/algorithms/SurfaceFactory.h"
#include "pmp/algorithms/SurfaceRemeshing.h"
#include "pmp/algorithms/SurfaceFeatures.h"
#include "pmp/algorithms/SurfaceSubdivision.h"
#include "pmp/algorithms/SurfaceTriangulation.h"

namespace pmp {

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

SurfaceMesh open_cone()
{
    auto mesh = SurfaceFactory::cone(8, 1, 1.5);
    for (auto f : mesh.faces())
        if (mesh.valence(f) > 3)
        {
            mesh.delete_face(f);
            mesh.garbage_collection();
            break;
        }
    return mesh;
}

SurfaceMesh texture_seams_mesh()
{
    SurfaceMesh mesh;
    auto v0 = mesh.add_vertex(Point(0.5999997854, 0.5196152329, 0.0000000000));
    auto v1 = mesh.add_vertex(Point(0.4499998093, 0.5196152329, -0.001000000));
    auto v2 = mesh.add_vertex(Point(0.2999998033, 0.5196152329, 0.0000000000));
    auto v3 = mesh.add_vertex(Point(0.6749998331, 0.3897114396, -0.001000000));
    auto v4 = mesh.add_vertex(Point(0.5249998569, 0.3897114396, 0.0000000000));
    auto v5 = mesh.add_vertex(Point(0.3749998510, 0.3897114396, 0.0000000000));
    auto v6 = mesh.add_vertex(Point(0.2249998450, 0.3897114396, 0.0000000000));
    auto v7 = mesh.add_vertex(Point(0.5999999046, 0.2598076165, 0.0000000000));
    auto v8 = mesh.add_vertex(Point(0.4499999285, 0.2598076165, 0.0000000000));
    auto v9 = mesh.add_vertex(Point(0.2999999225, 0.2598076165, 0.0000000000));
    auto v10 = mesh.add_vertex(Point(0.749999285, 0.2598076165, 0.0000000000));
    auto v11 = mesh.add_vertex(Point(0.8249998331, 0.3897114396, 0.0000000000));
    auto v12 = mesh.add_vertex(Point(0.749999285, 0.5196152329, 0.0000000000));
    auto v13 = mesh.add_vertex(Point(0.6749998331, 0.6496152329, 0.0000000000));
    auto v14 = mesh.add_vertex(Point(0.5249998569, 0.6496152329, 0.0000000000));
    auto v15 = mesh.add_vertex(Point(0.3749998510, 0.6496152329, 0.0000000000));

    mesh.add_triangle(v4, v0, v1);
    mesh.add_triangle(v4, v3, v0);
    mesh.add_triangle(v15, v4, v1);
    mesh.add_triangle(v2, v5, v4);
    mesh.add_triangle(v6, v5, v2);
    mesh.add_triangle(v7, v11, v4);
    mesh.add_triangle(v8, v7, v4);
    mesh.add_triangle(v8, v4, v5);
    mesh.add_triangle(v9, v8, v5);
    mesh.add_triangle(v9, v5, v6);
    mesh.add_triangle(v7, v10, v11);
    mesh.add_triangle(v4, v11, v3);
    mesh.add_triangle(v3, v11, v12);
    mesh.add_triangle(v3, v12, v0);
    mesh.add_triangle(v0, v12, v13);
    mesh.add_triangle(v0, v13, v14);
    mesh.add_triangle(v0, v14, v1);
    mesh.add_triangle(v1, v14, v15);
    mesh.add_triangle(v2, v4, v15);

    // add test texcoords
    auto texcoords = mesh.halfedge_property<Vector<Scalar, 2>>("h:tex");

    for (auto v : mesh.vertices())
    {
        Point p = mesh.position(v);
        for (auto h : mesh.halfedges(v))
        {
            if (mesh.is_boundary(mesh.opposite_halfedge(h)))
            {
                continue;
            }
            texcoords[mesh.opposite_halfedge(h)] = TexCoord(p[0], p[1]);
        }
    }

    // change texcoords to create a texture seam
    std::vector<Face> faces = {Face(0),  Face(1),  Face(12), Face(13),
                               Face(14), Face(15), Face(16), Face(17)};
    for (auto f : faces)
    {
        for (auto h : mesh.halfedges(f))
        {
            texcoords[h] += TexCoord(0.1, 0.1);
        }
    }

    return mesh;
}

} // namespace pmp