// Copyright 2017-2021 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#include "gtest/gtest.h"

#include "pmp/algorithms/parameterization.h"
#include "helpers.h"

using namespace pmp;

TEST(ParameterizationTest, parameterization)
{
    auto mesh = open_cone();
    harmonic_parameterization(mesh, false);
    harmonic_parameterization(mesh, true);
    auto tex = mesh.vertex_property<TexCoord>("v:tex");
    EXPECT_TRUE(tex);
}

TEST(ParameterizationTest, lscm)
{
    auto mesh = open_cone();
    lscm_parameterization(mesh);
    auto tex = mesh.vertex_property<TexCoord>("v:tex");
    EXPECT_TRUE(tex);
}
