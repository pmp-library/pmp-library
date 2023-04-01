// Copyright 2017-2021 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "gtest/gtest.h"

#include "pmp/algorithms/Decimation.h"
#include "pmp/algorithms/Features.h"
#include "Helpers.h"

using namespace pmp;

// plain simplification test
TEST(DecimationTest, simplification)
{
    auto mesh = subdivided_icosahedron();
    Features(mesh).clear();
    Decimation decimater(mesh);
    decimater.initialize(5,    // aspect ratio
                         0.5,  // edge length
                         10,   // max valence
                         10,   // normal deviation
                         0.1); // Hausdorff
    decimater.decimate(mesh.n_vertices() * 0.01);

    // use tolerance due to flakiness across OS versions
    EXPECT_NEAR(mesh.n_vertices(), size_t(101), 2);
}

// simplify with feature edge preservation enabled
TEST(DecimationTest, simplification_with_features)
{
    auto mesh = subdivided_icosahedron();
    Decimation decimater(mesh);
    decimater.initialize(5); // aspect ratio
    decimater.decimate(mesh.n_vertices() * 0.01);
    EXPECT_EQ(mesh.n_vertices(), size_t(12));
}

// simplify with respect to texture coordinates and seams
TEST(DecimationTest, simplification_texture_mesh)
{
    SurfaceMesh mesh = texture_seams_mesh();

    // if the test mesh does not have texcoords,
    // this test won't work
    ASSERT_TRUE(mesh.has_halfedge_property("h:tex"));

    Decimation decimater(mesh);
    decimater.initialize(10.0,  // aspect ratio
                         0.0,   // edge length
                         0.0,   // max valence
                         135.0, // normal deviation
                         0.0,   // Hausdorff
                         1e-2,  // seam threshold
                         1);    // seam angle deviation

    decimater.decimate(mesh.n_vertices() - 4);

    size_t seam_edges = 0;
    auto seams = mesh.get_edge_property<bool>("e:seam");
    for (auto e : mesh.edges())
        if (seams[e])
            seam_edges++;

    // test loop case 2
    auto se = mesh.find_edge(Vertex(4), Vertex(11));
    // test loop case 1
    auto se2 = mesh.find_edge(Vertex(4), Vertex(0));

    EXPECT_EQ(mesh.n_vertices(), size_t(12));
    EXPECT_EQ(seam_edges, size_t(13));
    EXPECT_EQ(seams[se], 1);
    EXPECT_EQ(seams[se2], 1);
}
