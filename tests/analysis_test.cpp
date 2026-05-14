// Copyright 2026 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#include "gtest/gtest.h"

#include "pmp/algorithms/analysis.h"
#include "pmp/algorithms/shapes.h"
#include "helpers.h"

using namespace pmp;

TEST(AnalysisTest, analyze_stats)
{
    const auto mesh = tetrahedron();
    const auto report = analyze(mesh);
    EXPECT_EQ(report.n_vertices, 4);
    EXPECT_EQ(report.n_edges, 6);
    EXPECT_EQ(report.n_faces, 4);
}

TEST(AnalysisTest, analyze_triangle_mesh)
{
    const auto mesh = tetrahedron();
    const auto report = analyze(mesh);
    EXPECT_TRUE(report.is_triangle_mesh);
}

TEST(AnalysisTest, analyze_quad_mesh)
{
    const auto mesh = hexahedron();
    const auto report = analyze(mesh);
    EXPECT_TRUE(report.is_quad_mesh);
}

TEST(AnalysisTest, analyze_polygon_mesh)
{
    const auto mesh = dodecahedron();
    const auto report = analyze(mesh);
    EXPECT_FALSE(report.is_triangle_mesh);
    EXPECT_FALSE(report.is_quad_mesh);
}

TEST(AnalysisTest, analyze_boundary)
{
    const auto mesh = plane();
    const auto report = analyze(mesh);
    EXPECT_TRUE(report.has_boundary);
}

TEST(AnalysisTest, analyze_manifoldness)
{
    const auto mesh = non_manifold_vertex();
    const auto report = analyze(mesh);
    EXPECT_FALSE(report.is_manifold);
}
