// Copyright 2017-2022 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "gtest/gtest.h"

#include "pmp/algorithms/subdivision.h"
#include "pmp/algorithms/Features.h"
#include "pmp/algorithms/shapes.h"
#include "Helpers.h"

using namespace pmp;

TEST(SubdivisionTest, loop_subdivision)
{
    auto mesh = shapes::icosahedron();
    subdivision::loop(mesh);
    EXPECT_EQ(mesh.n_faces(), size_t(80));
}

TEST(SubdivisionTest, loop_with_features)
{
    auto mesh = shapes::icosahedron();
    Features(mesh).detect_angle(25);
    subdivision::loop(mesh);
    EXPECT_EQ(mesh.n_faces(), size_t(80));
}

TEST(SubdivisionTest, loop_with_boundary)
{
    auto mesh = vertex_onering();
    subdivision::loop(mesh);
    EXPECT_EQ(mesh.n_faces(), size_t(24));
}

TEST(SubdivisionTest, catmull_clark_subdivision)
{
    auto mesh = shapes::hexahedron();
    subdivision::catmull_clark(mesh);
    EXPECT_EQ(mesh.n_faces(), size_t(24));
}

TEST(SubdivisionTest, catmull_clark_with_features)
{
    auto mesh = shapes::hexahedron();
    Features(mesh).detect_angle(25);
    subdivision::catmull_clark(mesh);
    EXPECT_EQ(mesh.n_faces(), size_t(24));
}

TEST(SubdivisionTest, catmull_clark_with_boundary)
{
    auto mesh = shapes::plane(1);
    subdivision::catmull_clark(mesh);
    EXPECT_EQ(mesh.n_faces(), size_t(4));
}

TEST(SubdivisionTest, quad_tri_on_quads)
{
    auto mesh = shapes::hexahedron();
    subdivision::quad_tri(mesh);
    EXPECT_EQ(mesh.n_faces(), size_t(24));
}

TEST(SubdivisionTest, quad_tri_on_triangles)
{
    auto mesh = shapes::tetrahedron();
    subdivision::quad_tri(mesh);
    EXPECT_EQ(mesh.n_faces(), size_t(16));
}

TEST(SubdivisionTest, quad_tri_on_mixed)
{
    auto mesh = shapes::cone(4); // pyramid
    subdivision::quad_tri(mesh);
    EXPECT_EQ(mesh.n_faces(), size_t(20));
}
