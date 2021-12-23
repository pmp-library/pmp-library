// Copyright 2017-2021 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "gtest/gtest.h"

#include "pmp/algorithms/SurfaceRemeshing.h"
#include "pmp/algorithms/SurfaceFeatures.h"
#include "pmp/algorithms/SurfaceFactory.h"
#include "pmp/algorithms/SurfaceTriangulation.h"

#include "Helpers.h"

using namespace pmp;

// adaptive remeshing
TEST(SurfaceRemeshingTest, adaptive_remeshing_with_features)
{
    auto mesh = SurfaceFactory::cylinder();
    SurfaceTriangulation(mesh).triangulate();
    SurfaceFeatures(mesh).detect_angle(25);
    auto bb = mesh.bounds().size();
    SurfaceRemeshing(mesh).adaptive_remeshing(0.001 * bb,  // min length
                                              1.0 * bb,    // max length
                                              0.001 * bb); // approx. error
    EXPECT_EQ(mesh.n_vertices(), 6u);
}

TEST(SurfaceRemeshingTest, adaptive_remeshing_with_boundary)
{
    // mesh with boundary
    auto mesh = open_cone();
    auto bb = mesh.bounds().size();
    SurfaceRemeshing(mesh).adaptive_remeshing(0.01 * bb,  // min length
                                              1.0 * bb,   // max length
                                              0.01 * bb); // approx. error
    EXPECT_EQ(mesh.n_vertices(), size_t(65));
}

TEST(SurfaceRemeshingTest, adaptive_remeshing_with_selection)
{
    auto mesh = SurfaceFactory::icosphere(1);

    // select half the vertices
    auto selected = mesh.add_vertex_property<bool>("v:selected");
    for (auto v : mesh.vertices())
        if (mesh.position(v)[1] > 0)
            selected[v] = true;

    auto bb = mesh.bounds().size();
    SurfaceRemeshing(mesh).adaptive_remeshing(0.01 * bb,  // min length
                                              1.0 * bb,   // max length
                                              0.01 * bb); // approx. error
    EXPECT_EQ(mesh.n_vertices(), size_t(62));
}

TEST(SurfaceRemeshingTest, uniform_remeshing)
{
    auto mesh = open_cone();
    SurfaceRemeshing(mesh).uniform_remeshing(0.5);
    EXPECT_EQ(mesh.n_vertices(), size_t(41));
}
