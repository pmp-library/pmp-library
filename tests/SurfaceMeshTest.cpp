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

TEST_F(SurfaceMeshTest, emptyMesh)
{
    EXPECT_EQ(mesh.nVertices(), size_t(0));
    EXPECT_EQ(mesh.nEdges(), size_t(0));
    EXPECT_EQ(mesh.nFaces(), size_t(0));
}

TEST_F(SurfaceMeshTest, insertRemoveSingleVertex)
{
    auto v0 = mesh.addVertex(Point(0, 0, 0));
    EXPECT_EQ(mesh.nVertices(), size_t(1));
    mesh.deleteVertex(v0);
    mesh.garbageCollection();
    EXPECT_EQ(mesh.nVertices(), size_t(0));
}

TEST_F(SurfaceMeshTest, insertRemoveSingleTriangle)
{
    auto v0 = mesh.addVertex(Point(0, 0, 0));
    auto v1 = mesh.addVertex(Point(1, 0, 0));
    auto v2 = mesh.addVertex(Point(0, 1, 0));
    auto f0 = mesh.addTriangle(v0, v1, v2);
    EXPECT_EQ(mesh.nVertices(), size_t(3));
    EXPECT_EQ(mesh.nEdges(), size_t(3));
    EXPECT_EQ(mesh.nFaces(), size_t(1));
    mesh.deleteFace(f0);
    mesh.garbageCollection();
    EXPECT_EQ(mesh.nVertices(), size_t(0));
    EXPECT_EQ(mesh.nEdges(), size_t(0));
    EXPECT_EQ(mesh.nFaces(), size_t(0));
}

TEST_F(SurfaceMeshTest, insertRemoveSingleQuad)
{
    auto v0 = mesh.addVertex(Point(0, 0, 0));
    auto v1 = mesh.addVertex(Point(1, 0, 0));
    auto v2 = mesh.addVertex(Point(1, 1, 0));
    auto v3 = mesh.addVertex(Point(0, 1, 0));
    auto f0 = mesh.addQuad(v0, v1, v2, v3);
    EXPECT_EQ(mesh.nVertices(), size_t(4));
    EXPECT_EQ(mesh.nEdges(), size_t(4));
    EXPECT_EQ(mesh.nFaces(), size_t(1));
    mesh.deleteFace(f0);
    mesh.garbageCollection();
    EXPECT_EQ(mesh.nVertices(), size_t(0));
    EXPECT_EQ(mesh.nEdges(), size_t(0));
    EXPECT_EQ(mesh.nFaces(), size_t(0));
}

TEST_F(SurfaceMeshTest, insertRemoveSinglePolygonalFace)
{
    std::vector<SurfaceMesh::Vertex> vertices(4);
    vertices[0] = mesh.addVertex(Point(0, 0, 0));
    vertices[1] = mesh.addVertex(Point(1, 0, 0));
    vertices[2] = mesh.addVertex(Point(1, 1, 0));
    vertices[3] = mesh.addVertex(Point(0, 1, 0));

    auto f0 = mesh.addFace(vertices);
    EXPECT_EQ(mesh.nVertices(), size_t(4));
    EXPECT_EQ(mesh.nEdges(), size_t(4));
    EXPECT_EQ(mesh.nFaces(), size_t(1));
    mesh.deleteFace(f0);
    mesh.garbageCollection();
    EXPECT_EQ(mesh.nVertices(), size_t(0));
    EXPECT_EQ(mesh.nEdges(), size_t(0));
    EXPECT_EQ(mesh.nFaces(), size_t(0));
}

TEST_F(SurfaceMeshTest, deleteCenterVertex)
{
    ASSERT_TRUE(mesh.read("pmp-data/off/vertex_onering.off"));
    EXPECT_EQ(mesh.nVertices(), size_t(7));
    EXPECT_EQ(mesh.nFaces(), size_t(6));
    SurfaceMesh::Vertex v0(3); // the central vertex
    mesh.deleteVertex(v0);
    mesh.garbageCollection();
    EXPECT_EQ(mesh.nVertices(), size_t(0));
    EXPECT_EQ(mesh.nFaces(), size_t(0));
}

TEST_F(SurfaceMeshTest, deleteCenterEdge)
{
    ASSERT_TRUE(mesh.read("pmp-data/off/edge_onering.off"));
    EXPECT_EQ(mesh.nVertices(), size_t(10));
    EXPECT_EQ(mesh.nFaces(), size_t(10));
    // the two vertices of the center edge
    SurfaceMesh::Vertex v0(4);
    SurfaceMesh::Vertex v1(5);

    auto e = mesh.findEdge(v0, v1);
    mesh.deleteEdge(e);
    mesh.garbageCollection();
    EXPECT_EQ(mesh.nVertices(), size_t(10));
    EXPECT_EQ(mesh.nFaces(), size_t(8));
}

TEST_F(SurfaceMeshTest, copy)
{
    auto v0 = mesh.addVertex(Point(0,0,0));
    auto v1 = mesh.addVertex(Point(1,0,0));
    auto v2 = mesh.addVertex(Point(0,1,0));
    mesh.addTriangle(v0,v1,v2);

    SurfaceMesh m2 = mesh;
    EXPECT_EQ(m2.nVertices(), size_t(3));
    EXPECT_EQ(m2.nEdges(), size_t(3));
    EXPECT_EQ(m2.nFaces(), size_t(1));
}

TEST_F(SurfaceMeshTest, assignment)
{
    auto v0 = mesh.addVertex(Point(0,0,0));
    auto v1 = mesh.addVertex(Point(1,0,0));
    auto v2 = mesh.addVertex(Point(0,1,0));
    mesh.addTriangle(v0,v1,v2);

    SurfaceMesh m2;
    m2.assign(mesh);
    EXPECT_EQ(m2.nVertices(), size_t(3));
    EXPECT_EQ(m2.nEdges(), size_t(3));
    EXPECT_EQ(m2.nFaces(), size_t(1));
}

TEST_F(SurfaceMeshTest, objectProperties)
{
    // explicit add
    auto midx = mesh.addObjectProperty<int>("m:idx");
    midx[0] = 0;
    EXPECT_EQ(mesh.objectProperties().size(), size_t(1));
    mesh.removeObjectProperty(midx);
    EXPECT_EQ(mesh.objectProperties().size(), size_t(0));

    // implicit add
    midx = mesh.objectProperty<int>("m:idx2");
    EXPECT_EQ(mesh.objectProperties().size(), size_t(1));
    mesh.removeObjectProperty(midx);
    EXPECT_EQ(mesh.objectProperties().size(), size_t(0));
}


TEST_F(SurfaceMeshTest, vertexProperties)
{
    addTriangle();

    auto osize = mesh.vertexProperties().size();

    // explicit add
    auto vidx = mesh.addVertexProperty<int>("v:idx");
    vidx[v0] = 0;
    EXPECT_EQ(mesh.vertexProperties().size(), osize+1);
    mesh.removeVertexProperty(vidx);
    EXPECT_EQ(mesh.vertexProperties().size(), osize);

    // implicit add
    vidx = mesh.vertexProperty<int>("v:idx2");
    EXPECT_EQ(mesh.vertexProperties().size(), osize+1);
    mesh.removeVertexProperty(vidx);
    EXPECT_EQ(mesh.vertexProperties().size(), osize);
}

TEST_F(SurfaceMeshTest, halfedgeProperties)
{
    addTriangle();
    // explicit add
    auto hidx = mesh.addHalfedgeProperty<int>("h:idx");
    auto h = mesh.halfedge(v0);
    hidx[h] = 0;
    EXPECT_EQ(mesh.halfedgeProperties().size(), size_t(2));
    mesh.removeHalfedgeProperty(hidx);
    EXPECT_EQ(mesh.halfedgeProperties().size(), size_t(1));

    // implicit add
    hidx = mesh.halfedgeProperty<int>("h:idx2");
    EXPECT_EQ(mesh.halfedgeProperties().size(), size_t(2));
    mesh.removeHalfedgeProperty(hidx);
    EXPECT_EQ(mesh.halfedgeProperties().size(), size_t(1));
}

TEST_F(SurfaceMeshTest, edgeProperties)
{
    addTriangle();
    // explicit add
    auto eidx = mesh.addEdgeProperty<int>("e:idx");
    auto e = mesh.edge(mesh.halfedge(v0));
    eidx[e] = 0;
    EXPECT_EQ(mesh.edgeProperties().size(), size_t(2));
    mesh.removeEdgeProperty(eidx);
    EXPECT_EQ(mesh.edgeProperties().size(), size_t(1));

    // implicit add
    eidx = mesh.edgeProperty<int>("e:idx2");
    EXPECT_EQ(mesh.edgeProperties().size(), size_t(2));
    mesh.removeEdgeProperty(eidx);
    EXPECT_EQ(mesh.edgeProperties().size(), size_t(1));
}

TEST_F(SurfaceMeshTest, faceProperties)
{
    addTriangle();
    // explicit add
    auto fidx = mesh.addFaceProperty<int>("f:idx");
    fidx[f0] = 0;
    EXPECT_EQ(mesh.faceProperties().size(), size_t(3));
    mesh.removeFaceProperty(fidx);
    EXPECT_EQ(mesh.faceProperties().size(), size_t(2));

    // implicit add
    fidx = mesh.faceProperty<int>("f:idx2");
    EXPECT_EQ(mesh.faceProperties().size(), size_t(3));
    mesh.removeFaceProperty(fidx);
    EXPECT_EQ(mesh.faceProperties().size(), size_t(2));
}

TEST_F(SurfaceMeshTest, vertexIterators)
{
    addTriangle();
    size_t sumIdx(0);
    for (auto v : mesh.vertices())
    {
        sumIdx += v.idx();
    }
    EXPECT_EQ(sumIdx, size_t(3));
}

TEST_F(SurfaceMeshTest, edgeIterators)
{
    addTriangle();
    size_t sumIdx(0);
    for (auto e : mesh.edges())
    {
        sumIdx += e.idx();
    }
    EXPECT_EQ(sumIdx, size_t(3));
}

TEST_F(SurfaceMeshTest, halfedgeIterators)
{
    addTriangle();
    size_t sumIdx(0);
    for (auto h : mesh.halfedges())
    {
        sumIdx += h.idx();
    }
    EXPECT_EQ(sumIdx, size_t(15));
}

TEST_F(SurfaceMeshTest, faceIterators)
{
    addTriangle();
    size_t sumIdx(0);
    for (auto f : mesh.faces())
    {
        sumIdx += f.idx();
    }
    EXPECT_EQ(sumIdx, size_t(0));
}


TEST_F(SurfaceMeshTest, isTriangleMesh)
{
    addTriangle();
    EXPECT_TRUE(mesh.isTriangleMesh());
}

TEST_F(SurfaceMeshTest, isQuadMesh)
{
    auto v0 = mesh.addVertex(Point(0,0,0));
    auto v1 = mesh.addVertex(Point(1,0,0));
    auto v2 = mesh.addVertex(Point(1,1,0));
    auto v3 = mesh.addVertex(Point(0,1,0));
    mesh.addQuad(v0,v1,v2,v3);
    EXPECT_TRUE(mesh.isQuadMesh());
    EXPECT_FALSE(mesh.isTriangleMesh());
    mesh.triangulate();
    EXPECT_TRUE(mesh.isTriangleMesh());
}

TEST_F(SurfaceMeshTest, polyMesh)
{
    std::vector<SurfaceMesh::Vertex> vertices(5);
    vertices[0] = mesh.addVertex(Point(0,0,0));
    vertices[1] = mesh.addVertex(Point(1,0,0));
    vertices[2] = mesh.addVertex(Point(1,1,0));
    vertices[3] = mesh.addVertex(Point(0.5,1,0));
    vertices[4] = mesh.addVertex(Point(0,1,0));
    mesh.addFace(vertices);
    EXPECT_FALSE(mesh.isTriangleMesh());
    EXPECT_FALSE(mesh.isQuadMesh());
    mesh.triangulate();
    EXPECT_TRUE(mesh.isTriangleMesh());
}

TEST_F(SurfaceMeshTest, valence)
{
    addTriangle();
    size_t sumValence(0);
    for (auto v : mesh.vertices())
    {
        sumValence += mesh.valence(v);
    }
    EXPECT_EQ(sumValence, size_t(6));
}

TEST_F(SurfaceMeshTest, collapse)
{
    addQuad();
    mesh.triangulate();
    EXPECT_EQ(mesh.nFaces(), size_t(2));
    auto h0  = mesh.findHalfedge(v3,v2);
    if (mesh.isCollapseOk(h0))
        mesh.collapse(h0);
    mesh.garbageCollection();
    EXPECT_EQ(mesh.nFaces(), size_t(1));
}

TEST_F(SurfaceMeshTest, faceSplit)
{
    addQuad();
    EXPECT_EQ(mesh.nFaces(), size_t(1));
    Point p(0.5,0.5,0);
    mesh.split(f0,p);
    EXPECT_EQ(mesh.nFaces(), size_t(4));
}

TEST_F(SurfaceMeshTest, edgeSplit)
{
    addTriangle();
    EXPECT_EQ(mesh.nFaces(), size_t(1));
    auto e = mesh.findEdge(v0,v1);
    Point p0 = mesh.position(v0);
    Point p1 = mesh.position(v1);
    Point p = (p0 + p1) * 0.5;
    mesh.split(e,p);
    EXPECT_EQ(mesh.nFaces(), size_t(2));
}

TEST_F(SurfaceMeshTest, edgeFlip)
{
    mesh.read("pmp-data/off/edge_onering.off");
    EXPECT_EQ(mesh.nVertices(), size_t(10));
    EXPECT_EQ(mesh.nFaces(), size_t(10));

    // the two vertices of the center edge
    SurfaceMesh::Vertex v0(4);
    SurfaceMesh::Vertex v1(5);
    auto e = mesh.findEdge(v0,v1);
    if (mesh.isFlipOk(e))
        mesh.flip(e);
    EXPECT_EQ(mesh.nVertices(), size_t(10));
    EXPECT_EQ(mesh.nFaces(), size_t(10));
}

TEST_F(SurfaceMeshTest, isManifold)
{
    mesh.read("pmp-data/off/vertex_onering.off");
    for (auto v : mesh.vertices())
        EXPECT_TRUE(mesh.isManifold(v));
}

TEST_F(SurfaceMeshTest, edgeLength)
{
    addQuad();
    Scalar sum(0);
    for (auto e : mesh.edges())
    {
        sum += mesh.edgeLength(e);
    }
    sum /= (Scalar)mesh.nEdges();
    //EXPECT_FLOAT_EQ(sum,0.52385628);

    std::cerr << "sum: " << sum << std::endl;
}

TEST_F(SurfaceMeshTest, propertyStats)
{
    mesh.propertyStats();
}

//=============================================================================
