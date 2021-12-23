// Copyright 2017-2021 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "gtest/gtest.h"

#include "pmp/algorithms/SurfaceHoleFilling.h"
#include "pmp/algorithms/SurfaceFactory.h"

#include "Helpers.h"

using namespace pmp;

Halfedge find_boundary(const SurfaceMesh& mesh)
{
    for (auto h : mesh.halfedges())
        if (mesh.is_boundary(h))
            return h;
    return Halfedge();
}

TEST(SurfaceHoleFillingTest, open_cone)
{
    // generate test mesh
    auto mesh = open_cone();

    // find boundary halfedge
    Halfedge h = find_boundary(mesh);
    EXPECT_TRUE(h.is_valid());

    // fill hole
    SurfaceHoleFilling hf(mesh);
    hf.fill_hole(h);

    // now we should not find a hole
    h = find_boundary(mesh);
    EXPECT_FALSE(h.is_valid());
}
