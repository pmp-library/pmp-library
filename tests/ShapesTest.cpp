// Copyright 2017-2021 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "gtest/gtest.h"

#include <pmp/algorithms/shapes.h>

using namespace pmp;

bool vertices_on_sphere(const SurfaceMesh& mesh)
{
    for (auto v : mesh.vertices())
    {
        const Scalar n = norm(mesh.position(v));
        if (n < 0.999 || n > 1.001)
        {
            return false;
        }
    }
    return true;
}

TEST(ShapesTest, tetrahedron)
{
    auto mesh = tetrahedron();
    EXPECT_EQ(mesh.n_vertices(), 4u);
    EXPECT_EQ(mesh.n_edges(), 6u);
    EXPECT_EQ(mesh.n_faces(), 4u);
    EXPECT_TRUE(mesh.is_triangle_mesh());
    EXPECT_TRUE(vertices_on_sphere(mesh));
}

TEST(ShapesTest, octahedron)
{
    auto mesh = octahedron();
    EXPECT_EQ(mesh.n_vertices(), 6u);
    EXPECT_EQ(mesh.n_edges(), 12u);
    EXPECT_EQ(mesh.n_faces(), 8u);
    EXPECT_TRUE(mesh.is_triangle_mesh());
    EXPECT_TRUE(vertices_on_sphere(mesh));
}

TEST(ShapesTest, hexahedron)
{
    auto mesh = hexahedron();
    EXPECT_EQ(mesh.n_vertices(), 8u);
    EXPECT_EQ(mesh.n_edges(), 12u);
    EXPECT_EQ(mesh.n_faces(), 6u);
    EXPECT_TRUE(mesh.is_quad_mesh());
    EXPECT_TRUE(vertices_on_sphere(mesh));
}

TEST(ShapesTest, icosahedron)
{
    auto mesh = icosahedron();
    EXPECT_EQ(mesh.n_vertices(), 12u);
    EXPECT_EQ(mesh.n_edges(), 30u);
    EXPECT_EQ(mesh.n_faces(), 20u);
    EXPECT_TRUE(mesh.is_triangle_mesh());
    EXPECT_TRUE(vertices_on_sphere(mesh));
}

TEST(ShapesTest, dodecahedron)
{
    auto mesh = dodecahedron();
    EXPECT_EQ(mesh.n_vertices(), 20u);
    EXPECT_EQ(mesh.n_edges(), 30u);
    EXPECT_EQ(mesh.n_faces(), 12u);
    EXPECT_FALSE(mesh.is_triangle_mesh());
    EXPECT_FALSE(mesh.is_quad_mesh());
    EXPECT_TRUE(vertices_on_sphere(mesh));
}

TEST(ShapesTest, icosphere)
{
    auto mesh = icosphere();
    EXPECT_EQ(mesh.n_vertices(), 642u);
    EXPECT_EQ(mesh.n_edges(), 1920u);
    EXPECT_EQ(mesh.n_faces(), 1280u);
    EXPECT_TRUE(mesh.is_triangle_mesh());
    EXPECT_TRUE(vertices_on_sphere(mesh));
}

TEST(ShapesTest, quad_sphere)
{
    auto mesh = quad_sphere();
    EXPECT_EQ(mesh.n_vertices(), 386u);
    EXPECT_EQ(mesh.n_edges(), 768u);
    EXPECT_EQ(mesh.n_faces(), 384u);
    EXPECT_TRUE(mesh.is_quad_mesh());
    EXPECT_TRUE(vertices_on_sphere(mesh));
}

TEST(ShapesTest, uv_sphere)
{
    auto mesh = uv_sphere();
    EXPECT_EQ(mesh.n_vertices(), 212u);
    EXPECT_EQ(mesh.n_edges(), 435u);
    EXPECT_EQ(mesh.n_faces(), 225u);
    EXPECT_FALSE(mesh.is_triangle_mesh());
    EXPECT_FALSE(mesh.is_quad_mesh());
    EXPECT_TRUE(vertices_on_sphere(mesh));
}

TEST(ShapesTest, plane_minimal)
{
    auto mesh = plane(1);
    EXPECT_EQ(mesh.n_vertices(), 4u);
    EXPECT_EQ(mesh.n_edges(), 4u);
    EXPECT_EQ(mesh.n_faces(), 1u);
    EXPECT_TRUE(mesh.is_quad_mesh());
}

TEST(ShapesTest, plane_planar)
{
    auto mesh = plane();
    Scalar z_sum = 0;
    for (auto v : mesh.vertices())
        z_sum += mesh.position(v)[2];
    EXPECT_EQ(z_sum, 0);
}

TEST(ShapesTest, plane_default)
{
    auto mesh = plane();
    EXPECT_EQ(mesh.n_vertices(), 25u);
    EXPECT_EQ(mesh.n_edges(), 40u);
    EXPECT_EQ(mesh.n_faces(), 16u);
}

TEST(ShapesTest, cone)
{
    auto mesh = cone(5);
    EXPECT_EQ(mesh.n_vertices(), 6u);
    EXPECT_EQ(mesh.n_edges(), 10u);
    EXPECT_EQ(mesh.n_faces(), 6u);
}

TEST(ShapesTest, cylinder)
{
    auto mesh = cylinder(3);
    EXPECT_EQ(mesh.n_vertices(), 6u);
    EXPECT_EQ(mesh.n_edges(), 9u);
    EXPECT_EQ(mesh.n_faces(), 5u);
}

TEST(ShapesTest, torus)
{
    auto mesh = torus();
    EXPECT_EQ(mesh.n_vertices(), 800u);
    EXPECT_EQ(mesh.n_edges(), 1600u);
    EXPECT_EQ(mesh.n_faces(), 800u);
    EXPECT_TRUE(mesh.is_quad_mesh());
}
