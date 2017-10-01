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
#include <pmp/algorithms/DifferentialGeometry.h>
#include <vector>

using namespace pmp;

class DifferentialGeometryTest : public ::testing::Test
{
public:
    SurfaceMesh         mesh;
    SurfaceMesh::Vertex v0, v1, v2, v3;
    SurfaceMesh::Face   f0;

    void addTriangle()
    {
        v0 = mesh.addVertex(Point(0, 0, 0));
        v1 = mesh.addVertex(Point(1, 0, 0));
        v2 = mesh.addVertex(Point(0, 1, 0));
        f0 = mesh.addTriangle(v0, v1, v2);
    }

    void oneRing()
    {
        ASSERT_TRUE(mesh.read("pmp-data/off/vertex_onering.off"));
        EXPECT_EQ(mesh.nVertices(), size_t(7));
        EXPECT_EQ(mesh.nFaces(), size_t(6));
        v0            = SurfaceMesh::Vertex(3); // the central vertex
        auto points   = mesh.getVertexProperty<Point>("v:point");
        points[v0][2] = 0.1; // lift central vertex
    }
};

TEST_F(DifferentialGeometryTest, triangleAreaPoints)
{
    addTriangle();
    Scalar area =
        triangleArea(mesh.position(v0), mesh.position(v1), mesh.position(v2));
    EXPECT_EQ(area, 0.5);
}

TEST_F(DifferentialGeometryTest, triangleAreaFace)
{
    addTriangle();
    Scalar area = triangleArea(mesh, f0);
    EXPECT_EQ(area, 0.5);
}

TEST_F(DifferentialGeometryTest, voronoiAreaBarycentric)
{
    oneRing();
    Scalar area = voronoiAreaBarycentric(mesh, v0);
    EXPECT_FLOAT_EQ(area, 0.049180791);
}

TEST_F(DifferentialGeometryTest, laplace)
{
    oneRing();
    auto lv = laplace(mesh, v0);
    EXPECT_GT(norm(lv), 0);
}

TEST_F(DifferentialGeometryTest, vertexCurvature)
{
    oneRing();
    auto vcurv = vertexCurvature(mesh, v0);
    EXPECT_FLOAT_EQ(vcurv.mean, 6.1538467);
    EXPECT_FLOAT_EQ(vcurv.gauss, 50.860939);
    EXPECT_FLOAT_EQ(vcurv.max, 6.1538467);
    EXPECT_FLOAT_EQ(vcurv.min, 6.1538467);
}
