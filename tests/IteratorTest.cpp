// Copyright 2011-2021 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "SurfaceMeshTest.h"
#include "Helpers.h"

#include <vector>

using namespace pmp;
using It = SurfaceMesh::VertexIterator;

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

TEST(VertexIteratorTest, misc)
{
    auto mesh = SurfaceMesh{};
    mesh.add_vertex(Point());
    mesh.add_vertex(Point());
    auto vit = mesh.vertices_begin();
    std::cout << (*vit).idx() << std::endl;
    std::cout << (*++vit).idx() << std::endl;
    std::cout << (*--vit).idx() << std::endl;
    vit--;
    std::cout << (*vit).idx() << std::endl;

    std::vector<Vertex> vertices;
    vertices.emplace_back(Vertex(3));

    vertices.insert(vertices.end(), mesh.vertices_begin(), mesh.vertices_end());

    for (auto v : vertices)
        std::cout << v.idx() << std::endl;
}

// test with std::distance to check if iterator types are well defined
TEST(VertexCirculatorTest, std_distance)
{
    auto mesh = vertex_onering();
    auto v = Vertex(3);
    auto vv = mesh.vertices(v);
    auto d = std::distance(vv.begin(), vv.end());
    EXPECT_EQ(d, 6);
}

TEST(VertexCirculatorTest, post_increment)
{
    auto mesh = vertex_onering();
    auto v = Vertex(3);
    auto vv = mesh.vertices(v);
    EXPECT_EQ((*vv++).idx(), 4u);
    EXPECT_EQ((*vv).idx(), 6u);
}

TEST(VertexCirculatorTest, post_decrement)
{
    auto mesh = vertex_onering();
    auto v = Vertex(3);
    auto vv = mesh.vertices(v);
    EXPECT_EQ((*vv--).idx(), 4u);
    EXPECT_EQ((*vv).idx(), 1u);
}