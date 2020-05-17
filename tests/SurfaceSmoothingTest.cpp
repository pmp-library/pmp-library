
// Copyright 2017-2019 the Polygon Mesh Processing Library developers.

// Distributed Distributed under a MIT-style license, see LICENSE.txt for details.

#include "gtest/gtest.h"

#include <pmp/algorithms/SurfaceSmoothing.h>
#include <pmp/algorithms/DifferentialGeometry.h>

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
    auto area_before = surface_area(mesh);
    SurfaceSmoothing ss(mesh);
    ss.implicit_smoothing(0.01, false, false);
    auto area_after = surface_area(mesh);
    EXPECT_LT(area_after, area_before);
}

TEST_F(SurfaceSmoothingTest, explicit_smoothing)
{
    auto area_before = surface_area(mesh);
    SurfaceSmoothing ss(mesh);
    ss.explicit_smoothing(10, true);
    ss.explicit_smoothing(10, false);
    auto area_after = surface_area(mesh);
    EXPECT_LT(area_after, area_before);
}
