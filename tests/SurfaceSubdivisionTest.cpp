//=============================================================================
// Copyright (C) 2017, 2018 The pmp-library developers
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

#include <pmp/algorithms/SurfaceSubdivision.h>
#include <pmp/algorithms/SurfaceFeatures.h>

using namespace pmp;

class SurfaceSubdivisionTest : public ::testing::Test
{
public:
    SurfaceSubdivisionTest()
    {
        EXPECT_TRUE(mesh.read("pmp-data/off/icosahedron_subdiv.off"));
    }
    SurfaceMesh mesh;
};

// plain loop subdivision
TEST_F(SurfaceSubdivisionTest, loopSubdivision)
{
    SurfaceSubdivision(mesh).loop();
    EXPECT_EQ(mesh.nVertices(),size_t(2562));
}

// loop subdivision with features
TEST_F(SurfaceSubdivisionTest, loopWithFeatures)
{
    SurfaceFeatures sf(mesh);
    sf.detectAngle(25);

    SurfaceSubdivision(mesh).loop();
    EXPECT_EQ(mesh.nVertices(),size_t(2562));
}

// loop subdivision with features
TEST_F(SurfaceSubdivisionTest, loopWithBoundary)
{
    mesh.clear();
    mesh.read("pmp-data/off/hemisphere.off");

    SurfaceSubdivision(mesh).loop();
    EXPECT_EQ(mesh.nVertices(),size_t(7321));
}

// Catmull-Clark subdivision on suzanne quad mesh
TEST_F(SurfaceSubdivisionTest, catmullClarkSubdivision)
{
    mesh.clear();
    mesh.read("pmp-data/obj/suzanne.obj");
    SurfaceSubdivision(mesh).catmullClark();
    EXPECT_EQ(mesh.nVertices(),size_t(2012));
}

// Catmull-Clark subdivision on fandisk quad mesh
TEST_F(SurfaceSubdivisionTest, catmullClarkWithFeatures)
{
    mesh.clear();
    mesh.read("pmp-data/off/fandisk_quads.off");

    SurfaceFeatures sf(mesh);
    sf.detectAngle(25);

    SurfaceSubdivision(mesh).catmullClark();
    EXPECT_EQ(mesh.nVertices(),size_t(3058));
}

// plain sqrt3 subdivision
TEST_F(SurfaceSubdivisionTest, sqrt3Subdivision)
{
    SurfaceSubdivision(mesh).sqrt3();
    EXPECT_EQ(mesh.nVertices(),size_t(1922));
}
