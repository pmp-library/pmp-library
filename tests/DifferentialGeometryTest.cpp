// Copyright 2017-2019 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "gtest/gtest.h"

#include <pmp/SurfaceMesh.h>
#include <pmp/algorithms/DifferentialGeometry.h>
#include <pmp/algorithms/SurfaceFactory.h>

#include <vector>

using namespace pmp;

class DifferentialGeometryTest : public ::testing::Test
{
public:
    SurfaceMesh mesh;
    static SurfaceMesh sphere;

    Vertex v0, v1, v2, v3;
    Face f0;

    void add_triangle()
    {
        v0 = mesh.add_vertex(Point(0, 0, 0));
        v1 = mesh.add_vertex(Point(1, 0, 0));
        v2 = mesh.add_vertex(Point(0, 1, 0));
        f0 = mesh.add_triangle(v0, v1, v2);
    }

    void one_ring()
    {
        ASSERT_TRUE(mesh.read("pmp-data/off/vertex_onering.off"));
        EXPECT_EQ(mesh.n_vertices(), size_t(7));
        EXPECT_EQ(mesh.n_faces(), size_t(6));
        v0 = Vertex(3); // the central vertex
        auto points = mesh.get_vertex_property<Point>("v:point");
        points[v0][2] = 0.1; // lift central vertex
    }
};

SurfaceMesh DifferentialGeometryTest::sphere = SurfaceFactory::icosphere(5);

TEST_F(DifferentialGeometryTest, triangle_areaPoints)
{
    add_triangle();
    Scalar area =
        triangle_area(mesh.position(v0), mesh.position(v1), mesh.position(v2));
    EXPECT_EQ(area, 0.5);
}

TEST_F(DifferentialGeometryTest, triangle_areaFace)
{
    add_triangle();
    Scalar area = triangle_area(mesh, f0);
    EXPECT_EQ(area, 0.5);
}

TEST_F(DifferentialGeometryTest, voronoi_area_barycentric)
{
    one_ring();
    Scalar area = voronoi_area_barycentric(mesh, v0);
    EXPECT_FLOAT_EQ(area, 0.024590395);
}

TEST_F(DifferentialGeometryTest, laplace)
{
    one_ring();
    auto lv = laplace(mesh, v0);
    EXPECT_GT(norm(lv), 0);
}

TEST_F(DifferentialGeometryTest, vertex_curvature)
{
    one_ring();
    auto vcurv = vertex_curvature(mesh, v0);
    EXPECT_FLOAT_EQ(vcurv.mean, 6.1538467);
    EXPECT_FLOAT_EQ(vcurv.gauss, 50.860939);
    EXPECT_FLOAT_EQ(vcurv.max, 6.1538467);
    EXPECT_FLOAT_EQ(vcurv.min, 6.1538467);
}

TEST_F(DifferentialGeometryTest, surface_area)
{
    auto area = surface_area(sphere);
    EXPECT_NEAR(area, 12.57, 1.0e-2);
}

TEST_F(DifferentialGeometryTest, volume)
{
    auto v = volume(sphere);
    EXPECT_NEAR(v, 4.18, 1.0e-2);
}

TEST_F(DifferentialGeometryTest, centroid)
{
    auto center = centroid(sphere);
    EXPECT_LT(norm(center), 1e-5);
}
