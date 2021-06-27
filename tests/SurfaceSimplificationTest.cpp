// Copyright 2017-2021 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "gtest/gtest.h"

#include "pmp/algorithms/SurfaceSimplification.h"
#include "pmp/algorithms/SurfaceFeatures.h"
#include "Helpers.h"

using namespace pmp;

// plain simplification test
TEST(SurfaceSimplificationTest, simplification)
{
    auto mesh = hemisphere();
    SurfaceSimplification ss(mesh);
    ss.initialize(5,    // aspect ratio
                  0.5,  // edge length
                  10,   // max valence
                  10,   // normal deviation
                  0.1); // Hausdorff
    ss.simplify(mesh.n_vertices() * 0.1);
    EXPECT_EQ(mesh.n_vertices(), size_t(188));
    EXPECT_EQ(mesh.n_faces(), size_t(351));
}

// simplify with feature edge preservation enabled
TEST(SurfaceSimplificationTest, simplification_with_features)
{
    auto mesh = subdivided_icosahedron();
    SurfaceSimplification ss(mesh);
    ss.initialize(5); // aspect ratio
    ss.simplify(mesh.n_vertices() * 0.1);
    EXPECT_EQ(mesh.n_vertices(), size_t(64));
}
