//=============================================================================
// Copyright (C) 2011-2018 The pmp-library developers
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

#include "SurfaceMeshTest.h"

#include <pmp/io/SurfaceMeshIO.h>
#include <pmp/algorithms/SurfaceNormals.h>
#include <vector>

using namespace pmp;

class SurfaceMeshIOTest : public SurfaceMeshTest
{
};

TEST_F(SurfaceMeshIOTest, polyIO)
{
    addTriangle();
    mesh.write("test.poly");
    mesh.clear();
    EXPECT_TRUE(mesh.isEmpty());
    mesh.read("test.poly");
    EXPECT_EQ(mesh.nVertices(), size_t(3));
    EXPECT_EQ(mesh.nFaces(), size_t(1));

    // check malformed file names
    EXPECT_FALSE(mesh.write("testpolyly"));
}

TEST_F(SurfaceMeshIOTest, objIO)
{
    addTriangle();
    SurfaceNormals::computeVertexNormals(mesh);
    mesh.addHalfedgeProperty<TextureCoordinate>("h:texcoord",TextureCoordinate(0,0));
    mesh.write("test.obj");
    mesh.clear();
    EXPECT_TRUE(mesh.isEmpty());
    mesh.read("test.obj");
    EXPECT_EQ(mesh.nVertices(), size_t(3));
    EXPECT_EQ(mesh.nFaces(), size_t(1));
}

TEST_F(SurfaceMeshIOTest, offIO)
{
    addTriangle();
    SurfaceNormals::computeVertexNormals(mesh);
    mesh.addVertexProperty<TextureCoordinate>("v:texcoord",TextureCoordinate(0,0));
    mesh.addVertexProperty<Color>("v:color",Color(0,0,0));
    IOOptions options(false, // binary
                      true, // normals
                      true, // colors
                      true); // texcoords
    mesh.write("test.off",options);
    mesh.clear();
    EXPECT_TRUE(mesh.isEmpty());
    mesh.read("test.off");
    EXPECT_EQ(mesh.nVertices(), size_t(3));
    EXPECT_EQ(mesh.nFaces(), size_t(1));
}

TEST_F(SurfaceMeshIOTest, offIOBinary)
{
    addTriangle();
    IOOptions options(true);
    mesh.write("binary.off", options);
    mesh.clear();
    EXPECT_TRUE(mesh.isEmpty());
    mesh.read("binary.off");
    EXPECT_EQ(mesh.nVertices(), size_t(3));
    EXPECT_EQ(mesh.nFaces(), size_t(1));
}

TEST_F(SurfaceMeshIOTest, stlIO)
{
    mesh.read("pmp-data/stl/icosahedron_ascii.stl");
    EXPECT_EQ(mesh.nVertices(), size_t(12));
    EXPECT_EQ(mesh.nFaces(), size_t(20));
    EXPECT_EQ(mesh.nEdges(), size_t(30));
    mesh.clear();
    mesh.read("pmp-data/stl/icosahedron_binary.stl");
    EXPECT_EQ(mesh.nVertices(), size_t(12));
    EXPECT_EQ(mesh.nFaces(), size_t(20));
    EXPECT_EQ(mesh.nEdges(), size_t(30));

    // try to write without normals being present
    EXPECT_FALSE(mesh.write("test.stl"));

    // the same with normals computed
    SurfaceNormals::computeFaceNormals(mesh);
    EXPECT_TRUE(mesh.write("test.stl"));

    // try to write non-triangle mesh
    mesh.clear();
    addQuad();
    EXPECT_FALSE(mesh.write("test.stl"));
}

TEST_F(SurfaceMeshIOTest, plyIO)
{
    addTriangle();
    mesh.write("test.ply");
    mesh.clear();
    EXPECT_TRUE(mesh.isEmpty());
    mesh.read("test.ply");
    EXPECT_EQ(mesh.nVertices(), size_t(3));
    EXPECT_EQ(mesh.nFaces(), size_t(1));
}

TEST_F(SurfaceMeshIOTest, plyBinaryIO)
{
    addTriangle();
    IOOptions options(true);
    mesh.write("binary.ply",options);
    mesh.clear();
    EXPECT_TRUE(mesh.isEmpty());
    mesh.read("binary.ply");
    EXPECT_EQ(mesh.nVertices(), size_t(3));
    EXPECT_EQ(mesh.nFaces(), size_t(1));
}
