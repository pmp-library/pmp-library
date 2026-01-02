// Copyright 2017-2021 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#include "gtest/gtest.h"

#include <pmp/algorithms/geodesics.h>
#include <pmp/algorithms/shapes.h>
#include <pmp/io/io.h>

using namespace pmp;

TEST(GeodesicsTest, geodesic)
{
    // generate unit sphere mesh
    SurfaceMesh mesh = icosphere(5);

    // compute geodesic distance from first vertex
    geodesics(mesh, std::vector<Vertex>{Vertex(0)});

    // find maximum geodesic distance
    Scalar d(0);
    auto distance = mesh.get_vertex_property<Scalar>("geodesic:distance");

    for (auto v : mesh.vertices())
        d = std::max(d, distance[v]);
    EXPECT_FLOAT_EQ(d, 3.1355045);

    // map distances to texture coordinates
    distance_to_texture_coordinates(mesh);
    auto tex = mesh.vertex_property<TexCoord>("v:tex");
    EXPECT_TRUE(tex);
}

TEST(GeodesicsTest, geodesic_symmetry)
{
    // read irregular mesh (to have virtual edges)
    SurfaceMesh mesh;
    read(mesh, "data/off/bunny_adaptive.off");

    Vertex v0, v1;
    Scalar d0, d1;

    // grow from first vector
    v0 = Vertex(0);
    geodesics(mesh, std::vector<Vertex>{v0});

    // find maximum geodesic distance
    d0 = 0;
    auto distance = mesh.get_vertex_property<Scalar>("geodesic:distance");

    for (auto v : mesh.vertices())
    {
        if (distance[v] > d0)
        {
            d0 = distance[v];
            v1 = v;
        }
    }

    // grow back from max-dist vertex to vertex 0
    geodesics(mesh, std::vector<Vertex>{v1});
    d1 = distance[v0];

    // expect both distance to be the same
    Scalar err = fabs(d0 - d1) / (0.5 * (d0 + d1));
    EXPECT_LT(err, 0.001);
}

TEST(GeodesicsTest, geodesic_maxnum)
{
    // generate unit sphere mesh
    SurfaceMesh mesh = icosphere(3);

    // compute geodesic distance from first vertex
    unsigned int maxnum = 42;
    std::vector<Vertex> neighbors;
    auto num =
        geodesics(mesh, std::vector<Vertex>{Vertex(0)},
                  std::numeric_limits<Scalar>::max(), maxnum, &neighbors);
    EXPECT_TRUE(num == maxnum);
    EXPECT_TRUE(neighbors.size() == maxnum);

    // test that neighbor array is properly sorted
    auto distance = mesh.get_vertex_property<Scalar>("geodesic:distance");
    for (unsigned int i = 0; i < neighbors.size() - 1; ++i)
    {
        EXPECT_TRUE(distance[neighbors[i]] <= distance[neighbors[i + 1]]);
    }
}
