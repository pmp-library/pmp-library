//=============================================================================
// Copyright (C) 2017, 2018 The pmp-library developers
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
// =============================================================================

#include "gtest/gtest.h"

#include <pmp/algorithms/SurfaceGeodesic.h>

using namespace pmp;

class SurfaceGeodesicTest : public ::testing::Test
{
public:
    SurfaceGeodesicTest()
    {
    }

    SurfaceMesh mesh;
};

TEST_F(SurfaceGeodesicTest, geodesic)
{
    // read mesh for unit sphere
    mesh.clear();
    EXPECT_TRUE(mesh.read("pmp-data/off/sphere.off"));

    // use first vertex as seed
    std::vector<SurfaceMesh::Vertex>  seed;
    seed.push_back(SurfaceMesh::Vertex(0));

    // compute geodesic distance
    SurfaceGeodesic geodist(mesh, seed);
    
    // find maximum geodesic distance
    Scalar d(0);
    for (auto v: mesh.vertices())
        d = std::max(d, geodist(v));
    EXPECT_FLOAT_EQ(d, 3.13061);
}

TEST_F(SurfaceGeodesicTest, geodesicToTexture)
{
    // read irregular mesh (to have virtual edges)
    mesh.clear();
    EXPECT_TRUE(mesh.read("pmp-data/off/bunny.off"));

    // use first vertex as seed
    std::vector<SurfaceMesh::Vertex>  seed;
    seed.push_back(SurfaceMesh::Vertex(0));

    // compute geodesic distance
    SurfaceGeodesic geodist(mesh, seed);
    
    // map distances to texture coordinates
    geodist.distanceToTextureCoordinates();
    auto tex = mesh.vertexProperty<TextureCoordinate>("v:tex");
    EXPECT_TRUE(tex);
}

