// Copyright 2017-2021 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#include "gtest/gtest.h"

#include "pmp/algorithms/numerics.h"
#include "pmp/io/io.h"

using namespace pmp;

TEST(NumericsTest, matrices_to_mesh)
{
    Eigen::MatrixXd V(3, 3);
    V << 0, 0, 0, 1, 0, 0, 0, 1, 0;
    Eigen::MatrixXi F(1, 3);
    F << 0, 1, 2;
    SurfaceMesh mesh;
    matrices_to_mesh(V, F, mesh);
    EXPECT_TRUE(mesh.n_vertices() == 3);
    EXPECT_TRUE(mesh.n_faces() == 1);
}

TEST(NumericsTest, mesh_to_matrices)
{
    SurfaceMesh mesh;
    auto v0 = mesh.add_vertex(Point(0, 0, 0));
    auto v1 = mesh.add_vertex(Point(1, 0, 0));
    auto v2 = mesh.add_vertex(Point(0, 1, 0));
    mesh.add_triangle(v0, v1, v2);

    Eigen::MatrixXd V;
    Eigen::MatrixXi F;
    mesh_to_matrices(mesh, V, F);
    EXPECT_TRUE(V.cols() == 3);
    EXPECT_TRUE(V.rows() == 3);
    EXPECT_TRUE(F.cols() == 3);
    EXPECT_TRUE(F.rows() == 1);
}