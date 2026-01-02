// Copyright 2017-2019 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#include "gtest/gtest.h"

#include <pmp/surface_mesh.h>
#include <pmp/algorithms/differential_geometry.h>
#include <pmp/algorithms/shapes.h>

#include "helpers.h"

#include <vector>

using namespace pmp;

class DifferentialGeometryTest : public ::testing::Test
{
public:
    SurfaceMesh mesh;
    static SurfaceMesh sphere;

    Vertex v0, v1, v2, v3;
    Face f0;

    Vertex central_vertex;

    void add_triangle()
    {
        v0 = mesh.add_vertex(Point(0, 0, 0));
        v1 = mesh.add_vertex(Point(1, 0, 0));
        v2 = mesh.add_vertex(Point(0, 1, 0));
        f0 = mesh.add_triangle(v0, v1, v2);
    }

    void one_ring()
    {
        mesh = vertex_onering();
        central_vertex = Vertex(3);             // the central vertex
        mesh.position(central_vertex)[2] = 0.1; // lift central vertex
    }
};

SurfaceMesh DifferentialGeometryTest::sphere = icosphere(5);

TEST_F(DifferentialGeometryTest, area_points)
{
    add_triangle();
    Scalar a =
        triangle_area(mesh.position(v0), mesh.position(v1), mesh.position(v2));
    EXPECT_EQ(a, 0.5);
}

TEST_F(DifferentialGeometryTest, area_face)
{
    add_triangle();
    Scalar a = face_area(mesh, f0);
    EXPECT_EQ(a, 0.5);
}

TEST_F(DifferentialGeometryTest, area_vertex)
{
    one_ring();
    Scalar a = voronoi_area(mesh, central_vertex);
    EXPECT_FLOAT_EQ(a, 0.024590395);
}

TEST_F(DifferentialGeometryTest, laplace)
{
    one_ring();
    auto lv = laplace(mesh, central_vertex);
    EXPECT_GT(norm(lv), 0);
}

TEST_F(DifferentialGeometryTest, area_surface)
{
    auto a = surface_area(sphere);
    EXPECT_NEAR(a, 12.57, 1.0e-2);
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
