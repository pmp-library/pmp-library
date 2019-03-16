//=============================================================================
// Copyright (C) 2017-2019 The pmp-library developers
//
// This file is part of the Polygon Mesh Processing Library.
// Distributed under the terms of the MIT license, see LICENSE.txt for details.
//
// SPDX-License-Identifier: MIT
//=============================================================================

#include "gtest/gtest.h"

#include <pmp/algorithms/HoleFilling.h>

using namespace pmp;

class HoleFillingTest : public ::testing::Test
{
public:
    HoleFillingTest()
    {
        EXPECT_TRUE(mesh.read("pmp-data/off/hemisphere.off"));
    }

    Halfedge find_boundary() const 
    {
        for (auto h: mesh.halfedges())
            if (mesh.is_boundary(h))
                return h;
        return Halfedge();
    }

    SurfaceMesh mesh;
};

TEST_F(HoleFillingTest, hemisphere)
{
    // find boundary halfedge
    Halfedge h = find_boundary();
    EXPECT_TRUE(h.is_valid());

    // fill hole
    HoleFilling hf(mesh);
    hf.fill_hole(h);

    // now we should not find a hole
    h = find_boundary();
    EXPECT_FALSE(h.is_valid());
}

