//=============================================================================
// Copyright (C) 2017-2019 The pmp-library developers
//
// This file is part of the Polygon Mesh Processing Library.
// Distributed under the terms of the MIT license, see LICENSE.txt for details.
//
// SPDX-License-Identifier: MIT
//=============================================================================

#include "gtest/gtest.h"

#include <pmp/algorithms/SurfaceFairing.h>

using namespace pmp;

class SurfaceFairingTest : public ::testing::Test
{
public:
    SurfaceFairingTest()
    {
        EXPECT_TRUE(mesh.read("pmp-data/off/icosahedron_subdiv.off"));
    }
    SurfaceMesh mesh;
};

TEST_F(SurfaceFairingTest, fairing)
{
    mesh.read("pmp-data/off/hemisphere.off");
    auto bbz = mesh.bounds().max()[2];
    SurfaceFairing sf(mesh);
    sf.fair();
    auto bbs = mesh.bounds().max()[2];
    EXPECT_LT(bbs,bbz);
}

TEST_F(SurfaceFairingTest, fairing_selected)
{
    mesh.read("pmp-data/off/sphere_low.off");
    auto bb = mesh.bounds();
    Scalar yrange = bb.max()[1] - bb.min()[1];
    auto vselected = mesh.vertex_property<bool>("v:selected",false);
    for (auto v : mesh.vertices())
    {
        auto p = mesh.position(v);
        if (p[1] >= (bb.max()[1] - 0.2*yrange))
        {
            vselected[v] = false;
        }
        else if (p[1] < (bb.max()[1] - 0.2*yrange) &&
                 p[1] > (bb.max()[1] - 0.8*yrange))
        {
            vselected[v] = true;
        }
        else
        {
            vselected[v] = false;
        }
    }
    SurfaceFairing sf(mesh);
    sf.fair();
    auto bb2 = mesh.bounds();
    EXPECT_LT(bb2.size(),bb.size());
}
