//=============================================================================
// Copyright (C) 2011-2019 The pmp-library developers
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

#include "SurfaceMeshTest.h"

#include <pmp/io/SurfaceMeshIO.h>
#include <pmp/algorithms/SurfaceNormals.h>
#include <vector>

using namespace pmp;

class SurfaceMeshIOTest : public SurfaceMeshTest
{
};

TEST_F(SurfaceMeshIOTest, poly_io)
{
    add_triangle();
    mesh.write("test.pmp");
    mesh.clear();
    EXPECT_TRUE(mesh.is_empty());
    mesh.read("test.pmp");
    EXPECT_EQ(mesh.n_vertices(), size_t(3));
    EXPECT_EQ(mesh.n_faces(), size_t(1));

    // check malformed file names
    EXPECT_FALSE(mesh.write("testpolyly"));
}

TEST_F(SurfaceMeshIOTest, obj_io)
{
    add_triangle();
    SurfaceNormals::compute_vertex_normals(mesh);
    mesh.add_halfedge_property<TextureCoordinate>("h:texcoord",TextureCoordinate(0,0));
    mesh.write("test.obj");
    mesh.clear();
    EXPECT_TRUE(mesh.is_empty());
    mesh.read("test.obj");
    EXPECT_EQ(mesh.n_vertices(), size_t(3));
    EXPECT_EQ(mesh.n_faces(), size_t(1));
}

TEST_F(SurfaceMeshIOTest, off_io)
{
    add_triangle();
    SurfaceNormals::compute_vertex_normals(mesh);
    mesh.add_vertex_property<TextureCoordinate>("v:texcoord",TextureCoordinate(0,0));
    mesh.add_vertex_property<Color>("v:color",Color(0,0,0));
    IOOptions options(false, // binary
                      true, // normals
                      true, // colors
                      true); // texcoords
    mesh.write("test.off",options);
    mesh.clear();
    EXPECT_TRUE(mesh.is_empty());
    mesh.read("test.off");
    EXPECT_EQ(mesh.n_vertices(), size_t(3));
    EXPECT_EQ(mesh.n_faces(), size_t(1));
}

TEST_F(SurfaceMeshIOTest, off_io_binary)
{
    add_triangle();
    IOOptions options(true);
    mesh.write("binary.off", options);
    mesh.clear();
    EXPECT_TRUE(mesh.is_empty());
    mesh.read("binary.off");
    EXPECT_EQ(mesh.n_vertices(), size_t(3));
    EXPECT_EQ(mesh.n_faces(), size_t(1));
}

TEST_F(SurfaceMeshIOTest, stl_io)
{
    mesh.read("pmp-data/stl/icosahedron_ascii.stl");
    EXPECT_EQ(mesh.n_vertices(), size_t(12));
    EXPECT_EQ(mesh.n_faces(), size_t(20));
    EXPECT_EQ(mesh.n_edges(), size_t(30));
    mesh.clear();
    mesh.read("pmp-data/stl/icosahedron_binary.stl");
    EXPECT_EQ(mesh.n_vertices(), size_t(12));
    EXPECT_EQ(mesh.n_faces(), size_t(20));
    EXPECT_EQ(mesh.n_edges(), size_t(30));

    // try to write without normals being present
    EXPECT_FALSE(mesh.write("test.stl"));

    // the same with normals computed
    SurfaceNormals::compute_face_normals(mesh);
    EXPECT_TRUE(mesh.write("test.stl"));

    // try to write non-triangle mesh
    mesh.clear();
    add_quad();
    EXPECT_FALSE(mesh.write("test.stl"));
}

TEST_F(SurfaceMeshIOTest, ply_io)
{
    add_triangle();
    mesh.write("test.ply");
    mesh.clear();
    EXPECT_TRUE(mesh.is_empty());
    mesh.read("test.ply");
    EXPECT_EQ(mesh.n_vertices(), size_t(3));
    EXPECT_EQ(mesh.n_faces(), size_t(1));
}

TEST_F(SurfaceMeshIOTest, ply_io_binary)
{
    add_triangle();
    IOOptions options(true);
    mesh.write("binary.ply",options);
    mesh.clear();
    EXPECT_TRUE(mesh.is_empty());
    mesh.read("binary.ply");
    EXPECT_EQ(mesh.n_vertices(), size_t(3));
    EXPECT_EQ(mesh.n_faces(), size_t(1));
}

TEST_F(SurfaceMeshIOTest, xyz_io)
{
    add_triangle();
    mesh.write("test.xyz");
    mesh.clear();
    EXPECT_TRUE(mesh.is_empty());
    mesh.read("test.xyz");
    EXPECT_EQ(mesh.n_vertices(), size_t(3));
}
