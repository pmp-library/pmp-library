// Copyright 2017-2021 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "gtest/gtest.h"

#include "pmp/algorithms/Parameterization.h"
#include "Helpers.h"

using namespace pmp;

TEST(ParameterizationTest, parameterization)
{
    auto mesh = open_cone();
    Parameterization param(mesh);
    param.harmonic(false);
    param.harmonic(true);
    auto tex = mesh.vertex_property<TexCoord>("v:tex");
    EXPECT_TRUE(tex);
}

TEST(ParameterizationTest, lscm)
{
    auto mesh = open_cone();
    Parameterization param(mesh);
    param.lscm();
    auto tex = mesh.vertex_property<TexCoord>("v:tex");
    EXPECT_TRUE(tex);
}
