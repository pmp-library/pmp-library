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

#include <pmp/algorithms/SurfaceRemeshing.h>
#include <pmp/algorithms/SurfaceFeatures.h>

using namespace pmp;

class SurfaceRemeshingTest : public ::testing::Test
{
public:
    SurfaceRemeshingTest()
    {
        EXPECT_TRUE(mesh.read("pmp-data/off/icosahedron_subdiv.off"));
    }
    SurfaceMesh mesh;
};

// adaptive remeshing
TEST_F(SurfaceRemeshingTest, adaptiveRemeshingWithFeatures)
{
    mesh.clear();
    mesh.read("pmp-data/off/fandisk.off");

    SurfaceFeatures sf(mesh);
    sf.detectAngle(25);

    auto bb = mesh.bounds().size();
    SurfaceRemeshing(mesh).adaptiveRemeshing(
        0.001 * bb,  // min length
        1.0 * bb,    // max length
        0.001 * bb, // approx. error
        1, // iterations
        false); // no projection
    EXPECT_EQ(mesh.nVertices(),size_t(3216));
}

TEST_F(SurfaceRemeshingTest, adaptiveRemeshingWithBoundary)
{
    // mesh with boundary
    mesh.clear();
    mesh.read("pmp-data/off/hemisphere.off");
    auto bb = mesh.bounds().size();
    SurfaceRemeshing(mesh).adaptiveRemeshing(
        0.001 * bb,  // min length
        1.0 * bb,    // max length
        0.001 * bb); // approx. error
    EXPECT_EQ(mesh.nVertices(),size_t(452));
}

TEST_F(SurfaceRemeshingTest, adaptiveRemeshingWithSelection)
{
    // mesh with boundary
    mesh.clear();
    mesh.read("pmp-data/off/hemisphere.off");

    // select half of the hemisphere
    auto selected = mesh.addVertexProperty<bool>("v:selected");
    for (auto v : mesh.vertices())
        if (mesh.position(v)[0] > 0.0)
        {
            selected[v] = true;
        }
    auto bb = mesh.bounds().size();
    SurfaceRemeshing(mesh).adaptiveRemeshing(
        0.001 * bb,  // min length
        1.0 * bb,    // max length
        0.001 * bb); // approx. error
    EXPECT_EQ(mesh.nVertices(),size_t(1182));
}

TEST_F(SurfaceRemeshingTest, uniformRemeshing)
{
    Scalar l(0);
    for (auto eit : mesh.edges())
        l += distance(mesh.position(mesh.vertex(eit, 0)),
                      mesh.position(mesh.vertex(eit, 1)));
    l /= (Scalar)mesh.nEdges();
    SurfaceRemeshing(mesh).uniformRemeshing(l);
    EXPECT_EQ(mesh.nVertices(),size_t(642));
}
