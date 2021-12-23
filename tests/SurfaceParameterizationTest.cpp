// Copyright 2017-2021 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "gtest/gtest.h"

#include "pmp/algorithms/SurfaceParameterization.h"
#include "Helpers.h"

using namespace pmp;

TEST(SurfaceParameterizationTest, parameterization)
{
    auto mesh = open_cone();
    SurfaceParameterization param(mesh);
    param.harmonic(false);
    param.harmonic(true);
    auto tex = mesh.vertex_property<TexCoord>("v:tex");
    EXPECT_TRUE(tex);
}

TEST(SurfaceParameterizationTest, lscm)
{
    auto mesh = open_cone();
    SurfaceParameterization param(mesh);
    param.lscm();
    auto tex = mesh.vertex_property<TexCoord>("v:tex");
    EXPECT_TRUE(tex);
}
