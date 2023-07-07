// Copyright 2017-2021 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "gtest/gtest.h"

#include "pmp/algorithms/fairing.h"
#include "pmp/algorithms/subdivision.h"
#include "pmp/algorithms/utilities.h"
#include "Helpers.h"

using namespace pmp;

TEST(FairingTest, fairing)
{
    auto mesh = open_cone();
    loop_subdivision(mesh);
    auto bbz = bounds(mesh).max()[2];
    fair(mesh);
    auto bbs = bounds(mesh).max()[2];
    EXPECT_LT(bbs, bbz);
}

TEST(FairingTest, fairing_selected)
{
    auto mesh = open_cone();
    loop_subdivision(mesh);

    auto bb = bounds(mesh);

    // select top vertices for fairing
    auto selected = mesh.vertex_property<bool>("v:selected");
    for (auto v : mesh.vertices())
        if (mesh.position(v)[2] > 0.5)
            selected[v] = true;

    fair(mesh);
    auto bb2 = bounds(mesh);
    EXPECT_LT(bb2.size(), bb.size());
}
