// Copyright 2017-2021 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#include "gtest/gtest.h"

#include "pmp/algorithms/hole_filling.h"
#include "pmp/algorithms/shapes.h"

#include "helpers.h"

using namespace pmp;

Halfedge find_boundary(const SurfaceMesh& mesh)
{
    for (auto h : mesh.halfedges())
        if (mesh.is_boundary(h))
            return h;
    return Halfedge();
}

TEST(HoleFillingTest, open_cone)
{
    // generate test mesh
    auto mesh = open_cone();

    // find boundary halfedge
    Halfedge h = find_boundary(mesh);
    EXPECT_TRUE(h.is_valid());

    // fill hole
    fill_hole(mesh, h);

    // now we should not find a hole
    h = find_boundary(mesh);
    EXPECT_FALSE(h.is_valid());
}
