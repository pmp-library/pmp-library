// Copyright 2017-2022 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "gtest/gtest.h"

#include "pmp/algorithms/Subdivision.h"
#include "pmp/algorithms/Features.h"
#include "pmp/algorithms/Shapes.h"
#include "Helpers.h"

using namespace pmp;

TEST(SubdivisionTest, loop_subdivision)
{
    auto mesh = Shapes::icosahedron();
    Subdivision(mesh).loop();
    EXPECT_EQ(mesh.n_faces(), size_t(80));
}

TEST(SubdivisionTest, loop_with_features)
{
    auto mesh = Shapes::icosahedron();
    Features(mesh).detect_angle(25);
    Subdivision(mesh).loop();
    EXPECT_EQ(mesh.n_faces(), size_t(80));
}

TEST(SubdivisionTest, loop_with_boundary)
{
    auto mesh = vertex_onering();
    Subdivision(mesh).loop();
    EXPECT_EQ(mesh.n_faces(), size_t(24));
}

TEST(SubdivisionTest, catmull_clark_subdivision)
{
    auto mesh = Shapes::hexahedron();
    Subdivision(mesh).catmull_clark();
    EXPECT_EQ(mesh.n_faces(), size_t(24));
}

TEST(SubdivisionTest, catmull_clark_with_features)
{
    auto mesh = Shapes::hexahedron();
    Features(mesh).detect_angle(25);
    Subdivision(mesh).catmull_clark();
    EXPECT_EQ(mesh.n_faces(), size_t(24));
}

TEST(SubdivisionTest, quad_tri_on_quads)
{
    auto mesh = Shapes::hexahedron();
    Subdivision(mesh).quad_tri();
    EXPECT_EQ(mesh.n_faces(), size_t(24));
}

TEST(SubdivisionTest, quad_tri_on_triangles)
{
    auto mesh = Shapes::tetrahedron();
    Subdivision(mesh).quad_tri();
    EXPECT_EQ(mesh.n_faces(), size_t(16));
}
