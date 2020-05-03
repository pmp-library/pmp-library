
// Copyright 2017-2019 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "gtest/gtest.h"

#include <pmp/algorithms/SurfaceParameterization.h>

using namespace pmp;

class SurfaceParameterizationTest : public ::testing::Test
{
public:
    SurfaceParameterizationTest()
    {
        EXPECT_TRUE(mesh.read("pmp-data/off/hemisphere.off"));
    }
    SurfaceMesh mesh;
};

TEST_F(SurfaceParameterizationTest, parameterization)
{
    SurfaceParameterization param(mesh);
    param.harmonic(false);
    param.harmonic(true);
    auto tex = mesh.vertex_property<TexCoord>("v:tex");
    EXPECT_TRUE(tex);
}

TEST_F(SurfaceParameterizationTest, lscm)
{
    SurfaceParameterization param(mesh);
    param.lscm();
    auto tex = mesh.vertex_property<TexCoord>("v:tex");
    EXPECT_TRUE(tex);
}
