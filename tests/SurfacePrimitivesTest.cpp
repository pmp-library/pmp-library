// Copyright 2017-2019 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "gtest/gtest.h"

#include <pmp/algorithms/SurfacePrimitives.h>

using namespace pmp;

bool vertices_on_sphere(const SurfaceMesh& mesh)
{
    for (auto v: mesh.vertices())
    {
        const Scalar n = norm(mesh.position(v));
        if (n < 0.999 || n > 1.001)
        {
            return false;
        }
    }
    return true;
}

TEST(SurfacePrimitivesTest, tetrahedron)
{
    auto mesh = tetrahedron();
    EXPECT_EQ(mesh.n_vertices(), 4u);
    EXPECT_EQ(mesh.n_edges(), 6u);
    EXPECT_EQ(mesh.n_faces(), 4u);
    EXPECT_TRUE(mesh.is_triangle_mesh());
    EXPECT_TRUE(vertices_on_sphere(mesh));
}

TEST(SurfacePrimitivesTest, octahedron)
{
    auto mesh = octahedron();
    EXPECT_EQ(mesh.n_vertices(), 6u);
    EXPECT_EQ(mesh.n_edges(), 12u);
    EXPECT_EQ(mesh.n_faces(), 8u);
    EXPECT_TRUE(mesh.is_triangle_mesh());
    EXPECT_TRUE(vertices_on_sphere(mesh));
}

TEST(SurfacePrimitivesTest, hexahedron)
{
    auto mesh = hexahedron();
    EXPECT_EQ(mesh.n_vertices(), 8u);
    EXPECT_EQ(mesh.n_edges(), 12u);
    EXPECT_EQ(mesh.n_faces(), 6u);
    EXPECT_TRUE(mesh.is_quad_mesh());
    EXPECT_TRUE(vertices_on_sphere(mesh));
}

TEST(SurfacePrimitivesTest, icosahedron)
{
    auto mesh = icosahedron();
    EXPECT_EQ(mesh.n_vertices(), 12u);
    EXPECT_EQ(mesh.n_edges(), 30u);
    EXPECT_EQ(mesh.n_faces(), 20u);
    EXPECT_TRUE(mesh.is_triangle_mesh());
    EXPECT_TRUE(vertices_on_sphere(mesh));
}

TEST(SurfacePrimitivesTest, dodecahedron)
{
    auto mesh = dodecahedron();
    EXPECT_EQ(mesh.n_vertices(), 20u);
    EXPECT_EQ(mesh.n_edges(), 30u);
    EXPECT_EQ(mesh.n_faces(), 12u);
    EXPECT_FALSE(mesh.is_triangle_mesh());
    EXPECT_FALSE(mesh.is_quad_mesh());
    EXPECT_TRUE(vertices_on_sphere(mesh));
}

TEST(SurfacePrimitivesTest, icosphere)
{
    auto mesh = icosphere();
    EXPECT_EQ(mesh.n_vertices(), 642u);
    EXPECT_EQ(mesh.n_edges(), 1920u);
    EXPECT_EQ(mesh.n_faces(), 1280u);
    EXPECT_TRUE(mesh.is_triangle_mesh());
    EXPECT_TRUE(vertices_on_sphere(mesh));
}

TEST(SurfacePrimitivesTest, quad_sphere)
{
    auto mesh = quad_sphere();
    EXPECT_EQ(mesh.n_vertices(), 386u);
    EXPECT_EQ(mesh.n_edges(), 768u);
    EXPECT_EQ(mesh.n_faces(), 384u);
    EXPECT_TRUE(mesh.is_quad_mesh());
    EXPECT_TRUE(vertices_on_sphere(mesh));
}

TEST(SurfacePrimitivesTest, uv_sphere)
{
    auto mesh = uv_sphere();
    EXPECT_EQ(mesh.n_vertices(), 212u);
    EXPECT_EQ(mesh.n_edges(), 435u);
    EXPECT_EQ(mesh.n_faces(), 225u);
    EXPECT_FALSE(mesh.is_triangle_mesh());
    EXPECT_FALSE(mesh.is_quad_mesh());
    EXPECT_TRUE(vertices_on_sphere(mesh));
}
