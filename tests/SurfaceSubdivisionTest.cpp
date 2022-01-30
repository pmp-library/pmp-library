// Copyright 2017-2022 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "gtest/gtest.h"

#include "pmp/algorithms/SurfaceSubdivision.h"
#include "pmp/algorithms/SurfaceFeatures.h"
#include "pmp/algorithms/SurfaceFactory.h"
#include "Helpers.h"

using namespace pmp;

TEST(SurfaceSubdivisionTest, loop_subdivision)
{
    auto mesh = SurfaceFactory::icosahedron();
    SurfaceSubdivision(mesh).loop();
    EXPECT_EQ(mesh.n_faces(), size_t(80));
}

TEST(SurfaceSubdivisionTest, loop_with_features)
{
    auto mesh = SurfaceFactory::icosahedron();
    SurfaceFeatures(mesh).detect_angle(25);
    SurfaceSubdivision(mesh).loop();
    EXPECT_EQ(mesh.n_faces(), size_t(80));
}

TEST(SurfaceSubdivisionTest, loop_with_boundary)
{
    auto mesh = vertex_onering();
    SurfaceSubdivision(mesh).loop();
    EXPECT_EQ(mesh.n_faces(), size_t(24));
}

TEST(SurfaceSubdivisionTest, catmull_clark_subdivision)
{
    auto mesh = SurfaceFactory::hexahedron();
    SurfaceSubdivision(mesh).catmull_clark();
    EXPECT_EQ(mesh.n_faces(), size_t(24));
}

TEST(SurfaceSubdivisionTest, catmull_clark_with_features)
{
    auto mesh = SurfaceFactory::hexahedron();
    SurfaceFeatures(mesh).detect_angle(25);
    SurfaceSubdivision(mesh).catmull_clark();
    EXPECT_EQ(mesh.n_faces(), size_t(24));
}

TEST(SurfaceSubdivisionTest, quad_tri_on_quads)
{
    auto mesh = SurfaceFactory::hexahedron();
    SurfaceSubdivision(mesh).quad_tri();
    EXPECT_EQ(mesh.n_faces(), size_t(24));
}

TEST(SurfaceSubdivisionTest, quad_tri_on_triangles)
{
    auto mesh = SurfaceFactory::tetrahedron();
    SurfaceSubdivision(mesh).quad_tri();
    EXPECT_EQ(mesh.n_faces(), size_t(16));
}
