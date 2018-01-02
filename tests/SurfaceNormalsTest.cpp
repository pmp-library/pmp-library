//=============================================================================
// Copyright (C) 2018 The pmp-library developers
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
#include <pmp/algorithms/SurfaceNormals.h>
#include <vector>

using namespace pmp;

class SurfaceNormalsTest : public ::testing::Test
{
public:
    SurfaceMesh mesh;
};

TEST_F(SurfaceNormalsTest, computeVertexNormals)
{
    mesh.read("pmp-data/stl/icosahedron_ascii.stl");
    SurfaceNormals::computeVertexNormals(mesh);
    auto vnormals = mesh.getVertexProperty<Normal>("v:normal");
    auto vn0 = vnormals[SurfaceMesh::Vertex(0)];
    EXPECT_GT(norm(vn0), 0);
}

TEST_F(SurfaceNormalsTest, computeFaceNormals)
{
    mesh.read("pmp-data/stl/icosahedron_ascii.stl");
    SurfaceNormals::computeFaceNormals(mesh);
    auto fnormals = mesh.getFaceProperty<Normal>("f:normal");
    auto fn0 = fnormals[SurfaceMesh::Face(0)];
    EXPECT_GT(norm(fn0), 0);
}

TEST_F(SurfaceNormalsTest, computeCornerNormal)
{
    mesh.read("pmp-data/stl/icosahedron_ascii.stl");
    auto h = SurfaceMesh::Halfedge(0);
    auto n = SurfaceNormals::computeCornerNormal(mesh,h,(Scalar)M_PI/3.0);
    EXPECT_GT(norm(n), 0);
}

TEST_F(SurfaceNormalsTest, polygonalFaceNormal)
{
    std::vector<SurfaceMesh::Vertex> vertices(5);
    vertices[0] = mesh.addVertex(Point(0,0,0));
    vertices[1] = mesh.addVertex(Point(1,0,0));
    vertices[2] = mesh.addVertex(Point(1,1,0));
    vertices[3] = mesh.addVertex(Point(0.5,1,0));
    vertices[4] = mesh.addVertex(Point(0,1,0));
    auto f0 = mesh.addFace(vertices);
    auto n0 = SurfaceNormals::computeFaceNormal(mesh,f0);
    EXPECT_GT(norm(n0), 0);
}
