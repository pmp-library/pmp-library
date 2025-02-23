// Copyright 2017-2021 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "gtest/gtest.h"

#include "pmp/surface_mesh.h"
#include "pmp/types.h"
#include "pmp/algorithms/utilities.h"
#include "surface_mesh_test.h"

using namespace pmp;

class UtilitiesTest : public SurfaceMeshTest
{
public:
    std::vector<IndexType> vertex_indices(Face f) const
    {
        std::vector<IndexType> vertices;
        for (auto v : mesh.vertices(f))
            vertices.push_back(v.idx());
        return vertices;
    }
};

TEST_F(UtilitiesTest, flip_faces)
{
    // test if vertex ordering is the same after flipping twice
    add_triangle();
    auto vertices_before = vertex_indices(f0);
    flip_faces(mesh);
    flip_faces(mesh);
    auto vertices_after = vertex_indices(f0);
    EXPECT_EQ(vertices_before, vertices_after);
}

TEST_F(UtilitiesTest, min_face_area)
{
    add_quad();
    EXPECT_EQ(min_face_area(mesh), 1.0);
}

TEST_F(UtilitiesTest, edge_length)
{
    add_quad();
    Edge e(0);
    auto length = edge_length(mesh, e);
    EXPECT_EQ(length, 1);
}

TEST_F(UtilitiesTest, mean_edge_length)
{
    add_quad();
    auto mean = mean_edge_length(mesh);
    EXPECT_EQ(mean, 1);
}

TEST_F(UtilitiesTest, connected_components)
{
    // add two faces with duplicate vertices
    add_triangle();
    add_quad();
    auto nc = connected_components(mesh);
    EXPECT_EQ(nc, 2);
}
