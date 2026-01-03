// Copyright 2021 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#include "gtest/gtest.h"

#include <pmp/algorithms/triangulation.h>
#include "helpers.h"

using namespace pmp;

TEST(TriangulationTest, triangulate)
{
    auto mesh = l_shape();
    EXPECT_FALSE(mesh.is_triangle_mesh());
    triangulate(mesh);
    EXPECT_TRUE(mesh.is_triangle_mesh());
}

TEST(TriangulationTest, min_area)
{
    auto mesh = l_shape();
    triangulate(mesh);
    EXPECT_EQ(mesh.n_faces(), size_t(10));
}
