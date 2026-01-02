// Copyright 2017-2021 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#include "gtest/gtest.h"

#include "pmp/algorithms/features.h"
#include "pmp/algorithms/shapes.h"
#include "helpers.h"

using namespace pmp;

TEST(FeaturesTest, detect_angle)
{
    auto mesh = hexahedron();
    auto nf = detect_features(mesh, 25);
    EXPECT_EQ(nf, 12u);
}

TEST(FeaturesTest, detect_boundary)
{
    auto mesh = vertex_onering();
    auto nb = detect_boundary(mesh);
    EXPECT_EQ(nb, 6u);
}

TEST(FeaturesTest, clear)
{
    auto mesh = vertex_onering();
    detect_boundary(mesh);
    clear_features(mesh);
    auto is_feature = mesh.get_edge_property<bool>("e:feature");
    for (auto e : mesh.edges())
        EXPECT_FALSE(is_feature[e]);
}
