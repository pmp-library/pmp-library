// Copyright 2017-2021 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "gtest/gtest.h"

#include "pmp/algorithms/SurfaceFairing.h"
#include "pmp/algorithms/SurfaceSubdivision.h"
#include "Helpers.h"

using namespace pmp;

TEST(SurfaceFairingTest, fairing)
{
    auto mesh = open_cone();
    SurfaceSubdivision(mesh).loop();
    auto bbz = mesh.bounds().max()[2];
    SurfaceFairing sf(mesh);
    sf.fair();
    auto bbs = mesh.bounds().max()[2];
    EXPECT_LT(bbs, bbz);
}

TEST(SurfaceFairingTest, fairing_selected)
{
    auto mesh = open_cone();
    SurfaceSubdivision(mesh).loop();

    auto bb = mesh.bounds();

    // select top vertices for fairing
    auto selected = mesh.vertex_property<bool>("v:selected");
    for (auto v : mesh.vertices())
        if (mesh.position(v)[2] > 0.5)
            selected[v] = true;

    SurfaceFairing sf(mesh);
    sf.fair();
    auto bb2 = mesh.bounds();
    EXPECT_LT(bb2.size(), bb.size());
}
