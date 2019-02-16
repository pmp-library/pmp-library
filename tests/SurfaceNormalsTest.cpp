//=============================================================================
// Copyright (C) 2018-2019 The pmp-library developers
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

#include <pmp/algorithms/SurfaceNormals.h>
#include <vector>

using namespace pmp;

class SurfaceNormalsTest : public ::testing::Test
{
public:
    SurfaceMesh mesh;
};

TEST_F(SurfaceNormalsTest, compute_vertex_normals)
{
    mesh.read("pmp-data/stl/icosahedron_ascii.stl");
    SurfaceNormals::compute_vertex_normals(mesh);
    auto vnormals = mesh.get_vertex_property<Normal>("v:normal");
    auto vn0 = vnormals[Vertex(0)];
    EXPECT_GT(norm(vn0), 0);
}

TEST_F(SurfaceNormalsTest, compute_face_normals)
{
    mesh.read("pmp-data/stl/icosahedron_ascii.stl");
    SurfaceNormals::compute_face_normals(mesh);
    auto fnormals = mesh.get_face_property<Normal>("f:normal");
    auto fn0 = fnormals[Face(0)];
    EXPECT_GT(norm(fn0), 0);
}

TEST_F(SurfaceNormalsTest, compute_corner_normal)
{
    mesh.read("pmp-data/stl/icosahedron_ascii.stl");
    auto h = Halfedge(0);
    auto n = SurfaceNormals::compute_corner_normal(mesh,h,(Scalar)M_PI/3.0);
    EXPECT_GT(norm(n), 0);
}

TEST_F(SurfaceNormalsTest, polygonal_face_normal)
{
    std::vector<Vertex> vertices(5);
    vertices[0] = mesh.add_vertex(Point(0,0,0));
    vertices[1] = mesh.add_vertex(Point(1,0,0));
    vertices[2] = mesh.add_vertex(Point(1,1,0));
    vertices[3] = mesh.add_vertex(Point(0.5,1,0));
    vertices[4] = mesh.add_vertex(Point(0,1,0));
    auto f0 = mesh.add_face(vertices);
    auto n0 = SurfaceNormals::compute_face_normal(mesh,f0);
    EXPECT_GT(norm(n0), 0);
}
