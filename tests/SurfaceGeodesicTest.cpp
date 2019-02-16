//=============================================================================
// Copyright (C) 2017-2019 The pmp-library developers
//
// This file is part of the Polygon Mesh Processing Library.
// Distributed under the terms of the MIT license, see LICENSE.txt for details.
//
// SPDX-License-Identifier: MIT
//=============================================================================

#include "gtest/gtest.h"

#include <pmp/algorithms/SurfaceGeodesic.h>

using namespace pmp;

TEST(SurfaceGeodesicTest, geodesic)
{
    // read mesh for unit sphere
    SurfaceMesh mesh;
    EXPECT_TRUE(mesh.read("pmp-data/off/sphere.off"));

    // use first vertex as seed
    std::vector<Vertex> seed;
    seed.push_back(Vertex(0));

    // compute geodesic distance
    SurfaceGeodesic geodist(mesh, seed);

    // find maximum geodesic distance
    Scalar d(0);
    for (auto v: mesh.vertices())
        d = std::max(d, geodist(v));
    EXPECT_FLOAT_EQ(d, 3.13061);
}

TEST(SurfaceGeodesicTest, geodesic_to_texture)
{
    // read irregular mesh (to have virtual edges)
    SurfaceMesh mesh;
    EXPECT_TRUE(mesh.read("pmp-data/off/bunny_adaptive.off"));

    // use first vertex as seed
    std::vector<Vertex> seed;
    seed.push_back(Vertex(0));

    // compute geodesic distance
    SurfaceGeodesic geodist(mesh, seed);

    // map distances to texture coordinates
    geodist.distance_to_texture_coordinates();
    auto tex = mesh.vertex_property<TextureCoordinate>("v:tex");
    EXPECT_TRUE(tex);
}
