// Copyright 2026 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#include "gtest/gtest.h"

#include "pmp/tolerances.h"
#include "pmp/algorithms/analysis.h"
#include "pmp/algorithms/shapes.h"
#include "helpers.h"

using namespace pmp;

TEST(AnalysisTest, analyze_stats)
{
    const auto mesh = tetrahedron();
    const auto analysis = analyze(mesh);
    EXPECT_EQ(analysis.n_vertices, 4);
    EXPECT_EQ(analysis.n_edges, 6);
    EXPECT_EQ(analysis.n_faces, 4);
}

TEST(AnalysisTest, analyze_triangle_mesh)
{
    const auto mesh = tetrahedron();
    const auto analysis = analyze(mesh);
    EXPECT_TRUE(analysis.is_triangle_mesh);
}

TEST(AnalysisTest, analyze_quad_mesh)
{
    const auto mesh = hexahedron();
    const auto analysis = analyze(mesh);
    EXPECT_TRUE(analysis.is_quad_mesh);
}

TEST(AnalysisTest, analyze_polygon_mesh)
{
    const auto mesh = dodecahedron();
    const auto analysis = analyze(mesh);
    EXPECT_FALSE(analysis.is_triangle_mesh);
    EXPECT_FALSE(analysis.is_quad_mesh);
}

TEST(AnalysisTest, analyze_boundary)
{
    const auto mesh = plane();
    const auto analysis = analyze(mesh);
    EXPECT_TRUE(analysis.has_boundary);
}

TEST(AnalysisTest, analyze_manifoldness)
{
    const auto mesh = non_manifold_vertex();
    const auto analysis = analyze(mesh);
    EXPECT_FALSE(analysis.is_manifold);
}

TEST(AnalysisTest, analyze_components)
{
    auto mesh = plane();
    mesh.add_vertex(Point(0, 0, 1)); // add isolated vertex
    const auto analysis = analyze(mesh);
    EXPECT_EQ(analysis.n_components, 2);
}

TEST(AnalysisTest, analyze_isolated_vertices)
{
    SurfaceMesh mesh;
    mesh.add_vertex(Point(1, 0, 0));
    mesh.add_vertex(Point(0, 0, 1));
    const auto analysis = analyze(mesh);
    EXPECT_EQ(analysis.n_isolated_vertices, 2);
}

TEST(AnalysisTest, analyze_zero_area_face)
{
    SurfaceMesh mesh;
    auto v0 = mesh.add_vertex(Point(0, 0, 0));
    auto v1 = mesh.add_vertex(Point(1, 0, 0));
    auto v2 = mesh.add_vertex(Point(2, 0, 0));
    mesh.add_face({v0, v1, v2});
    const auto analysis = analyze(mesh);
    EXPECT_EQ(analysis.n_degenerate_faces, 1);
}

TEST(AnalysisTest, analyze_short_edge_triangle)
{
    SurfaceMesh mesh;
    auto v0 = mesh.add_vertex(Point(0, 0, 0));
    auto v1 = mesh.add_vertex(Point(1e-15, 0, 0));
    auto v2 = mesh.add_vertex(Point(0, 1e-15, 0));
    mesh.add_face({v0, v1, v2});
    const auto analysis = analyze(mesh);
    EXPECT_EQ(analysis.n_degenerate_faces, 1);
}

TEST(AnalysisTest, analyze_short_edge_quad)
{
    SurfaceMesh mesh;
    auto v0 = mesh.add_vertex(Point(0, 0, 0));
    auto v1 = mesh.add_vertex(Point(1e-15, 0, 0));
    auto v2 = mesh.add_vertex(Point(0, 1e-15, 0));
    auto v3 = mesh.add_vertex(Point(1e-15, 1e-15, 0));
    mesh.add_face({v0, v1, v2, v3});
    const auto analysis = analyze(mesh);
    EXPECT_EQ(analysis.n_degenerate_faces, 1);
}

TEST(AnalysisTest, analyze_duplicate_vertices)
{
    SurfaceMesh mesh;
    const auto tol = detail::absolute_epsilon<Scalar>();
    auto v0 = mesh.add_vertex(Point(0, 0, 0));
    auto v1 = mesh.add_vertex(Point(0.5 * tol, 0, 0));
    auto v2 = mesh.add_vertex(Point(0, 0.5 * tol, 0));
    mesh.add_face({v0, v1, v2});
    const auto analysis = analyze(mesh);
    EXPECT_EQ(analysis.n_duplicate_vertices, 3);
}

TEST(AnalysisTest, analyze_duplicate_vertices_outside_tolerance)
{
    SurfaceMesh mesh;
    const auto tol = detail::absolute_epsilon<Scalar>();
    auto v0 = mesh.add_vertex(Point(0, 0, 0));
    auto v1 = mesh.add_vertex(Point(2 * tol, 0, 0));
    auto v2 = mesh.add_vertex(Point(0, 4 * tol, 0));
    mesh.add_face({v0, v1, v2});
    const auto analysis = analyze(mesh);
    EXPECT_EQ(analysis.n_duplicate_vertices, 0);
}
