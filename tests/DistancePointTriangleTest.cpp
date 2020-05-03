
// Copyright 2017-2019 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "gtest/gtest.h"

#include <pmp/SurfaceMesh.h>
#include <pmp/algorithms/DistancePointTriangle.h>
#include <vector>

using namespace pmp;

class DistancePointTriangleTest : public ::testing::Test
{
public:
    SurfaceMesh mesh;
    Vertex v0, v1, v2, v3;
    Face f0;

    void add_degenerate_triangle()
    {
        v0 = mesh.add_vertex(Point(0, 0, 0));
        v1 = mesh.add_vertex(Point(1, 0, 0));
        v2 = mesh.add_vertex(Point(0, 0, 0));
        f0 = mesh.add_triangle(v0, v1, v2);
    }
};

TEST_F(DistancePointTriangleTest, distance_point_degenerate_triangle)
{
    add_degenerate_triangle();
    Point p(0, 1, 0);
    Point nearest;
    Scalar dist = dist_point_triangle(p, mesh.position(v0), mesh.position(v1),
                                      mesh.position(v2), nearest);
    EXPECT_FLOAT_EQ(dist, 1.0);
    EXPECT_EQ(nearest, Point(0, 0, 0));
}
