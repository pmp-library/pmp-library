// Copyright 2021 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "gtest/gtest.h"

#include <pmp/algorithms/SurfaceTriangulation.h>
#include "Helpers.h"

using namespace pmp;

TEST(SurfaceTriangulationTest, min_area)
{
    auto mesh = l_shape();
    SurfaceTriangulation tr(mesh);
    tr.triangulate(SurfaceTriangulation::Objective::MIN_AREA);
    EXPECT_EQ(mesh.n_faces(), size_t(10));
}

TEST(SurfaceTriangulationTest, max_angle)
{
    auto mesh = l_shape();
    SurfaceTriangulation tr(mesh);
    tr.triangulate(SurfaceTriangulation::Objective::MAX_ANGLE);
    EXPECT_EQ(mesh.n_faces(), size_t(10));
}