// Copyright 2017-2021 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "gtest/gtest.h"

#include "pmp/algorithms/SurfaceRemeshing.h"
#include "pmp/algorithms/SurfaceFeatures.h"
#include "Helpers.h"

using namespace pmp;

// adaptive remeshing
TEST(SurfaceRemeshingTest, adaptive_remeshing_with_features)
{
    SurfaceMesh mesh;
    mesh.read("pmp-data/off/fandisk.off");

    SurfaceFeatures sf(mesh);
    sf.detect_angle(25);

    auto bb = mesh.bounds().size();
    SurfaceRemeshing(mesh).adaptive_remeshing(0.001 * bb, // min length
                                              1.0 * bb,   // max length
                                              0.001 * bb, // approx. error
                                              1,          // iterations
                                              false);     // no projection
    EXPECT_EQ(mesh.n_vertices(), size_t(845));
}

TEST(SurfaceRemeshingTest, adaptive_remeshing_with_boundary)
{
    // mesh with boundary
    auto mesh = hemisphere();
    auto bb = mesh.bounds().size();
    SurfaceRemeshing(mesh).adaptive_remeshing(0.001 * bb,  // min length
                                              1.0 * bb,    // max length
                                              0.001 * bb); // approx. error
    EXPECT_EQ(mesh.n_vertices(), size_t(769));
}

TEST(SurfaceRemeshingTest, adaptive_remeshing_with_selection)
{
    // mesh with boundary
    auto mesh = hemisphere();

    // select half of the hemisphere
    auto selected = mesh.add_vertex_property<bool>("v:selected");
    for (auto v : mesh.vertices())
        if (mesh.position(v)[0] > 0.0)
            selected[v] = true;

    auto bb = mesh.bounds().size();

    // adaptive remeshing with large approx error and max length to obtain a
    // clear difference in selected region
    SurfaceRemeshing(mesh).adaptive_remeshing(0.001 * bb, // min length
                                              5.0 * bb,   // max length
                                              0.01 * bb); // approx. error
    EXPECT_EQ(mesh.n_vertices(), size_t(500));
}

TEST(SurfaceRemeshingTest, uniform_remeshing)
{
    // mesh with boundary
    auto mesh = hemisphere();

    // compute mean edge length
    Scalar l(0);
    for (auto eit : mesh.edges())
        l += distance(mesh.position(mesh.vertex(eit, 0)),
                      mesh.position(mesh.vertex(eit, 1)));
    l /= (Scalar)mesh.n_edges();

    SurfaceRemeshing(mesh).uniform_remeshing(l);
    EXPECT_EQ(mesh.n_vertices(), size_t(925));
}
