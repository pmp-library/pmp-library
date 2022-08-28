// Copyright 2011-2021 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "SurfaceMeshTest.h"
#include "Helpers.h"

#include <vector>

using namespace pmp;
using It = SurfaceMesh::VertexIterator;

// Note: These are partial tests only used for development. They are not meant
// to fully cover iterators and their functionality,

TEST(VertexIteratorTest, default_constructible)
{
    It a;
    EXPECT_FALSE((*a).is_valid());
}

TEST(VertexIteratorTest, copy_constructible)
{
    It a;
    It b(a);
    EXPECT_FALSE((*b).is_valid());
}

TEST(VertexIteratorTest, assignable)
{
    It a;
    auto b = a;
    EXPECT_FALSE((*b).is_valid());
}

TEST(VertexIteratorTest, insert)
{
    auto mesh = SurfaceMesh{};
    auto vertices = std::vector<Vertex>{};
    mesh.add_vertex(Point(0));
    vertices.insert(vertices.end(), mesh.vertices_begin(), mesh.vertices_end());
    EXPECT_EQ(vertices.size(), 1u);
}

TEST(VertexCirculatorTest, std_distance)
{
    auto mesh = vertex_onering();
    auto v = Vertex(3); // center vertex
    auto vv = mesh.vertices(v);
    auto d = std::distance(vv.begin(), vv.end());
    EXPECT_EQ(d, 6);
}

TEST(VertexCirculatorTest, post_increment)
{
    auto mesh = vertex_onering();
    auto v = Vertex(3); // center vertex
    auto vv = mesh.vertices(v);
    EXPECT_EQ((*vv++).idx(), 4u);
    EXPECT_EQ((*vv).idx(), 6u);
}

TEST(VertexCirculatorTest, post_decrement)
{
    auto mesh = vertex_onering();
    auto v = Vertex(3); // center vertex
    auto vv = mesh.vertices(v);
    EXPECT_EQ((*vv--).idx(), 4u);
    EXPECT_EQ((*vv).idx(), 1u);
}