//=============================================================================
// Copyright (C) 2017-2019 The pmp-library developers
//
// This file is part of the Polygon Mesh Processing Library.
// Distributed under a MIT-style license, see LICENSE.txt for details.
//
// SPDX-License-Identifier: MIT-with-employer-disclaimer
//=============================================================================

#include "gtest/gtest.h"

#include <pmp/algorithms/SurfaceHoleFilling.h>

using namespace pmp;

class SurfaceHoleFillingTest : public ::testing::Test
{
public:
    SurfaceHoleFillingTest()
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

TEST_F(SurfaceHoleFillingTest, hemisphere)
{
    // find boundary halfedge
    Halfedge h = find_boundary();
    EXPECT_TRUE(h.is_valid());

    // fill hole
    SurfaceHoleFilling hf(mesh);
    hf.fill_hole(h);

    // now we should not find a hole
    h = find_boundary();
    EXPECT_FALSE(h.is_valid());
}
