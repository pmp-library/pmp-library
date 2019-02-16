//=============================================================================
// Copyright (C) 2017-2019 The pmp-library developers
//
// This file is part of the Polygon Mesh Processing Library.
// Distributed under the terms of the MIT license, see LICENSE.txt for details.
//
// SPDX-License-Identifier: MIT
//=============================================================================

#include "gtest/gtest.h"

#include <pmp/algorithms/SurfaceFeatures.h>

using namespace pmp;

class SurfaceFeaturesTest : public ::testing::Test
{
public:
    SurfaceFeaturesTest()
    {
        EXPECT_TRUE(mesh.read("pmp-data/off/icosahedron_subdiv.off"));
    }
    SurfaceMesh mesh;
};

// feature angle
TEST_F(SurfaceFeaturesTest, detect_feature_angle)
{
    SurfaceFeatures sf(mesh);
    sf.detect_angle(25);

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
TEST_F(SurfaceFeaturesTest, detect_boundary)
{
    mesh.clear();
    mesh.read("pmp-data/off/vertex_onering.off");
    SurfaceFeatures sf(mesh);
    sf.detect_boundary();

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
