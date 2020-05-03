
// Copyright 2017-2019 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "gtest/gtest.h"

#include <pmp/algorithms/SurfaceRemeshing.h>
#include <pmp/algorithms/SurfaceFeatures.h>

using namespace pmp;

class SurfaceRemeshingTest : public ::testing::Test
{
public:
    SurfaceRemeshingTest()
    {
        EXPECT_TRUE(mesh.read("pmp-data/off/icosahedron_subdiv.off"));
    }
    SurfaceMesh mesh;
};

// adaptive remeshing
TEST_F(SurfaceRemeshingTest, adaptive_remeshing_with_features)
{
    mesh.clear();
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

TEST_F(SurfaceRemeshingTest, adaptive_remeshing_with_boundary)
{
    // mesh with boundary
    mesh.clear();
    mesh.read("pmp-data/off/hemisphere.off");
    auto bb = mesh.bounds().size();
    SurfaceRemeshing(mesh).adaptive_remeshing(0.001 * bb,  // min length
                                              1.0 * bb,    // max length
                                              0.001 * bb); // approx. error
    EXPECT_EQ(mesh.n_vertices(), size_t(463));
}

TEST_F(SurfaceRemeshingTest, adaptive_remeshing_with_selection)
{
    // mesh with boundary
    mesh.clear();
    mesh.read("pmp-data/off/hemisphere.off");

    // select half of the hemisphere
    auto selected = mesh.add_vertex_property<bool>("v:selected");
    for (auto v : mesh.vertices())
        if (mesh.position(v)[0] > 0.0)
        {
            selected[v] = true;
        }
    auto bb = mesh.bounds().size();
    SurfaceRemeshing(mesh).adaptive_remeshing(0.001 * bb,  // min length
                                              1.0 * bb,    // max length
                                              0.001 * bb); // approx. error
    EXPECT_EQ(mesh.n_vertices(), size_t(1184));
}

TEST_F(SurfaceRemeshingTest, uniform_remeshing)
{
    Scalar l(0);
    for (auto eit : mesh.edges())
        l += distance(mesh.position(mesh.vertex(eit, 0)),
                      mesh.position(mesh.vertex(eit, 1)));
    l /= (Scalar)mesh.n_edges();
    SurfaceRemeshing(mesh).uniform_remeshing(l);
    EXPECT_EQ(mesh.n_vertices(), size_t(642));
}
