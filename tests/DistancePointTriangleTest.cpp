//=============================================================================
// Copyright (C) 2017 The pmp-library developers
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

#include <pmp/SurfaceMesh.h>
#include <pmp/algorithms/distancePointTriangle.h>
#include <vector>

using namespace pmp;

class DistancePointTriangleTest : public ::testing::Test
{
public:
    SurfaceMesh         mesh;
    SurfaceMesh::Vertex v0, v1, v2, v3;
    SurfaceMesh::Face   f0;

    void addDegenerateTriangle()
    {
        v0 = mesh.addVertex(Point(0, 0, 0));
        v1 = mesh.addVertex(Point(1, 0, 0));
        v2 = mesh.addVertex(Point(0, 0, 0));
        f0 = mesh.addTriangle(v0, v1, v2);
    }
};

TEST_F(DistancePointTriangleTest, distancePointDegenerateTriangle)
{
    addDegenerateTriangle();
    Point  p(0, 1, 0);
    Point  nearest;
    Scalar dist = distPointTriangle(p, mesh.position(v0), mesh.position(v1),
                                    mesh.position(v2), nearest);
    EXPECT_FLOAT_EQ(dist, 1.0);
    EXPECT_EQ(nearest, Point(0, 0, 0));
}
