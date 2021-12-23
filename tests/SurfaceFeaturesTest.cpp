// Copyright 2017-2021 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "gtest/gtest.h"

#include "pmp/algorithms/SurfaceFeatures.h"
#include "pmp/algorithms/SurfaceFactory.h"
#include "Helpers.h"

using namespace pmp;

TEST(SurfaceFeaturesTest, detect_angle)
{
    auto mesh = SurfaceFactory::hexahedron();
    auto nf = SurfaceFeatures(mesh).detect_angle(25);
    EXPECT_EQ(nf, 12u);
}

TEST(SurfaceFeaturesTest, detect_boundary)
{
    auto mesh = vertex_onering();
    auto features = SurfaceFeatures(mesh);
    auto nb = features.detect_boundary();
    EXPECT_EQ(nb, 6u);
}

TEST(SurfaceFeaturesTest, clear)
{
    auto mesh = vertex_onering();
    auto features = SurfaceFeatures(mesh);
    features.detect_boundary();
    features.clear();
    auto is_feature = mesh.get_edge_property<bool>("e:feature");
    for (auto e : mesh.edges())
        EXPECT_FALSE(is_feature[e]);
}
