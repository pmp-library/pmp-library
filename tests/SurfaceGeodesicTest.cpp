
// Copyright 2017-2019 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "gtest/gtest.h"

#include <pmp/algorithms/SurfaceGeodesic.h>

using namespace pmp;

TEST(SurfaceGeodesicTest, geodesic)
{
    // read mesh for unit sphere
    SurfaceMesh mesh;
    EXPECT_TRUE(mesh.read("pmp-data/off/sphere.off"));

    // compute geodesic distance from first vertex
    SurfaceGeodesic geodist(mesh);
    geodist.compute(std::vector<Vertex>{Vertex(0)});

    // find maximum geodesic distance
    Scalar d(0);
    for (auto v : mesh.vertices())
        d = std::max(d, geodist(v));
    EXPECT_FLOAT_EQ(d, 3.1348989);

    // map distances to texture coordinates
    geodist.distance_to_texture_coordinates();
    auto tex = mesh.vertex_property<TexCoord>("v:tex");
    EXPECT_TRUE(tex);
}

TEST(SurfaceGeodesicTest, geodesic_symmetry)
{
    // read irregular mesh (to have virtual edges)
    SurfaceMesh mesh;
    EXPECT_TRUE(mesh.read("pmp-data/off/bunny_adaptive.off"));

    SurfaceGeodesic geodist(mesh);
    Vertex v0, v1;
    Scalar d0, d1;

    // grow from first vector
    v0 = Vertex(0);
    geodist.compute(std::vector<Vertex>{v0});

    // find maximum geodesic distance
    d0 = 0;
    for (auto v : mesh.vertices())
    {
        if (geodist(v) > d0)
        {
            d0 = geodist(v);
            v1 = v;
        }
    }

    // grow back from max-dist vertex to vertex 0
    geodist.compute(std::vector<Vertex>{v1});
    d1 = geodist(v0);

    // expect both distance to be the same
    Scalar err = fabs(d0 - d1) / (0.5 * (d0 + d1));
    EXPECT_LT(err, 0.001);
}

TEST(SurfaceGeodesicTest, geodesic_maxnum)
{
    // read mesh for unit sphere
    SurfaceMesh mesh;
    EXPECT_TRUE(mesh.read("pmp-data/off/sphere.off"));

    // compute geodesic distance from first vertex
    unsigned int maxnum = 42;
    unsigned int num;
    SurfaceGeodesic geodist(mesh);
    std::vector<Vertex> neighbors;
    num =
        geodist.compute(std::vector<Vertex>{Vertex(0)},
                        std::numeric_limits<Scalar>::max(), maxnum, &neighbors);
    EXPECT_TRUE(num == maxnum);
    EXPECT_TRUE(neighbors.size() == maxnum);

    // test for another seed
    num =
        geodist.compute(std::vector<Vertex>{Vertex(12345)},
                        std::numeric_limits<Scalar>::max(), maxnum, &neighbors);
    EXPECT_TRUE(num == maxnum);
    EXPECT_TRUE(neighbors.size() == maxnum);

    // test that neighbor array is properly sorted
    for (unsigned int i = 0; i < neighbors.size() - 1; ++i)
    {
        EXPECT_TRUE(geodist(neighbors[i]) <= geodist(neighbors[i + 1]));
    }
}
