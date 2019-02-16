//=============================================================================
// Copyright (C) 2017-2019 The pmp-library developers
//
// This file is part of the Polygon Mesh Processing Library.
// Distributed under the terms of the MIT license, see LICENSE.txt for details.
//
// SPDX-License-Identifier: MIT
//=============================================================================

#include "gtest/gtest.h"

#include <pmp/algorithms/SurfaceSimplification.h>
#include <pmp/algorithms/SurfaceFeatures.h>

using namespace pmp;

class SurfaceSimplificationTest : public ::testing::Test
{
public:
    SurfaceSimplificationTest()
    {
        EXPECT_TRUE(mesh.read("pmp-data/off/icosahedron_subdiv.off"));
    }
    SurfaceMesh mesh;
};

// plain simplification test
TEST_F(SurfaceSimplificationTest, simplification)
{
    mesh.clear();
    mesh.read("pmp-data/off/bunny_adaptive.off");
    SurfaceSimplification ss(mesh);
    ss.initialize(5, // aspect ratio
                  0.01, // edge length
                  10, // max valence
                  10, // normal deviation
                  0.001); // Hausdorff
    ss.simplify(mesh.n_vertices() * 0.1);
    EXPECT_EQ(mesh.n_vertices(),size_t(3800));
    EXPECT_EQ(mesh.n_faces(),size_t(7596));
}

// simplify with feature edge preservation enabled
TEST_F(SurfaceSimplificationTest, simplification_with_features)
{
    SurfaceFeatures sf(mesh);
    sf.detect_angle(25);

    SurfaceSimplification ss(mesh);
    ss.initialize(5); // aspect ratio
    ss.simplify(mesh.n_vertices() * 0.1);
    EXPECT_EQ(mesh.n_vertices(),size_t(64));
}
