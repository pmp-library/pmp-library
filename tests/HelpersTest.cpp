// Copyright 2021 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "gtest/gtest.h"

#include "Helpers.h"

using namespace pmp;

TEST(Helpers, vertex_onering)
{
    auto mesh = vertex_onering();
    EXPECT_EQ(mesh.n_vertices(), size_t(7));
    EXPECT_EQ(mesh.n_faces(), size_t(6));
}
