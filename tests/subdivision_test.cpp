// Copyright 2017-2022 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "gtest/gtest.h"

#include "pmp/algorithms/subdivision.h"
#include "pmp/algorithms/features.h"
#include "pmp/algorithms/shapes.h"
#include "helpers.h"

using namespace pmp;

TEST(SubdivisionTest, loop_subdivision)
{
    auto mesh = icosahedron();
    loop_subdivision(mesh);
    EXPECT_EQ(mesh.n_faces(), size_t(80));
}

TEST(SubdivisionTest, loop_with_features)
{
    auto mesh = icosahedron();
    detect_features(mesh, 25);
    loop_subdivision(mesh);
    EXPECT_EQ(mesh.n_faces(), size_t(80));
}

TEST(SubdivisionTest, loop_with_boundary)
{
    auto mesh = vertex_onering();
    loop_subdivision(mesh);
    EXPECT_EQ(mesh.n_faces(), size_t(24));
}

TEST(SubdivisionTest, catmull_clark_subdivision)
{
    auto mesh = hexahedron();
    catmull_clark_subdivision(mesh);
    EXPECT_EQ(mesh.n_faces(), size_t(24));
}

TEST(SubdivisionTest, catmull_clark_with_features)
{
    auto mesh = hexahedron();
    detect_features(mesh, 25);
    catmull_clark_subdivision(mesh);
    EXPECT_EQ(mesh.n_faces(), size_t(24));
}

TEST(SubdivisionTest, catmull_clark_with_boundary)
{
    auto mesh = plane(1);
    catmull_clark_subdivision(mesh);
    EXPECT_EQ(mesh.n_faces(), size_t(4));
}

TEST(SubdivisionTest, quad_tri_on_quads)
{
    auto mesh = hexahedron();
    quad_tri_subdivision(mesh);
    EXPECT_EQ(mesh.n_faces(), size_t(24));
}

TEST(SubdivisionTest, quad_tri_on_triangles)
{
    auto mesh = tetrahedron();
    quad_tri_subdivision(mesh);
    EXPECT_EQ(mesh.n_faces(), size_t(16));
}

TEST(SubdivisionTest, quad_tri_on_mixed)
{
    auto mesh = cone(4); // pyramid
    quad_tri_subdivision(mesh);
    EXPECT_EQ(mesh.n_faces(), size_t(20));
}
