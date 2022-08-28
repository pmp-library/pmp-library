// Copyright 2017-2021 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "gtest/gtest.h"

#include "pmp/algorithms/Features.h"
#include "pmp/algorithms/Shapes.h"
#include "Helpers.h"

using namespace pmp;

TEST(FeaturesTest, detect_angle)
{
    auto mesh = Shapes::hexahedron();
    auto nf = Features(mesh).detect_angle(25);
    EXPECT_EQ(nf, 12u);
}

TEST(FeaturesTest, detect_boundary)
{
    auto mesh = vertex_onering();
    auto features = Features(mesh);
    auto nb = features.detect_boundary();
    EXPECT_EQ(nb, 6u);
}

TEST(FeaturesTest, clear)
{
    auto mesh = vertex_onering();
    auto features = Features(mesh);
    features.detect_boundary();
    features.clear();
    auto is_feature = mesh.get_edge_property<bool>("e:feature");
    for (auto e : mesh.edges())
        EXPECT_FALSE(is_feature[e]);
}
