// Copyright 2017-2021 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "gtest/gtest.h"

#include "pmp/algorithms/SurfaceFeatures.h"
#include "Helpers.h"

using namespace pmp;

// feature angle
TEST(SurfaceFeaturesTest, detect_feature_angle)
{
    auto mesh = subdivided_icosahedron();

    SurfaceFeatures sf(mesh);
    auto nf = sf.detect_angle(25);
    EXPECT_EQ(nf, 240u);

    auto efeature = mesh.get_edge_property<bool>("e:feature");
    bool found = false;
    for (auto e : mesh.edges())
        if (efeature[e])
        {
            found = true;
            break;
        }
    EXPECT_TRUE(found);
    sf.clear();
    found = false;
    for (auto e : mesh.edges())
        if (efeature[e])
        {
            found = true;
            break;
        }
    EXPECT_FALSE(found);
}

// boundary edges
TEST(SurfaceFeaturesTest, detect_boundary)
{
    auto mesh = vertex_onering();
    SurfaceFeatures sf(mesh);
    auto nb = sf.detect_boundary();
    EXPECT_EQ(nb, 6u);

    auto efeature = mesh.get_edge_property<bool>("e:feature");
    bool found = false;
    for (auto e : mesh.edges())
        if (efeature[e])
        {
            found = true;
            break;
        }
    EXPECT_TRUE(found);
}
