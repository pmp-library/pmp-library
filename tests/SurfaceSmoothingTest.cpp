//=============================================================================
// Copyright (C) 2017-2019 The pmp-library developers
//
// This file is part of the Polygon Mesh Processing Library.
// Distributed Distributed under the terms of the MIT license, see LICENSE.txt for details.
//
// SPDX-License-Identifier: MIT
//=============================================================================

#include "gtest/gtest.h"

#include <pmp/algorithms/SurfaceSmoothing.h>

using namespace pmp;

class SurfaceSmoothingTest : public ::testing::Test
{
public:
    SurfaceSmoothingTest()
    {
        EXPECT_TRUE(mesh.read("pmp-data/off/hemisphere.off"));
    }
    SurfaceMesh mesh;
};

TEST_F(SurfaceSmoothingTest, implicit_smoothing)
{
    auto bbz = mesh.bounds().max()[2];
    SurfaceSmoothing ss(mesh);
    ss.implicit_smoothing(0.01);
    ss.implicit_smoothing(0.01,true);
    auto bbs = mesh.bounds().max()[2];
    EXPECT_LT(bbs,bbz);
}

TEST_F(SurfaceSmoothingTest, explicit_smoothing)
{
    auto bbz = mesh.bounds().max()[2];
    SurfaceSmoothing ss(mesh);
    ss.explicit_smoothing(10);
    ss.explicit_smoothing(10,true);
    auto bbs = mesh.bounds().max()[2];
    EXPECT_LT(bbs,bbz);
}
