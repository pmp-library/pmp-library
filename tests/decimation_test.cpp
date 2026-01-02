// Copyright 2017-2021 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#include "gtest/gtest.h"

#include "pmp/algorithms/decimation.h"
#include "pmp/algorithms/features.h"
#include "helpers.h"

using namespace pmp;

// plain simplification test
TEST(DecimationTest, simplification)
{
    auto mesh = subdivided_icosahedron();
    clear_features(mesh);
    decimate(mesh, mesh.n_vertices() * 0.01,
             5,    // aspect ratio
             0.5,  // edge length
             10,   // max valence
             10,   // normal deviation
             0.1); // Hausdorff
    // use tolerance due to flakiness across OS versions
    EXPECT_NEAR(mesh.n_vertices(), size_t(101), 2);
}

// simplify with feature edge preservation enabled
TEST(DecimationTest, simplification_with_features)
{
    auto mesh = subdivided_icosahedron();
    decimate(mesh, mesh.n_vertices() * 0.01, 5 /* aspect ratio */);
    EXPECT_EQ(mesh.n_vertices(), size_t(12));
}

// simplify with respect to texture coordinates and seams
TEST(DecimationTest, simplification_texture_mesh)
{
    SurfaceMesh mesh = texture_seams_mesh();

    // if the test mesh does not have texcoords,
    // this test won't work
    ASSERT_TRUE(mesh.has_halfedge_property("h:tex"));

    auto seam_threshold = 1.0e-2;
    decimate(mesh, mesh.n_vertices() - 4,
             10.0,           // aspect ratio
             0.0,            // edge length
             0.0,            // max valence
             135.0,          // normal deviation
             0.0,            // Hausdorff
             seam_threshold, // seam threshold
             1);             // seam angle deviation

    size_t seam_edges = 0;
    auto texcoords = mesh.get_halfedge_property<TexCoord>("h:tex");
    auto seams = mesh.add_edge_property<bool>("e:seam");
    if (texcoords)
    {
        for (auto e : mesh.edges())
        {
            // texcoords are stored in halfedge pointing towards a vertex
            Halfedge h0 = mesh.halfedge(e, 0);
            Halfedge h1 = mesh.halfedge(e, 1);     //opposite halfedge
            Halfedge h0p = mesh.prev_halfedge(h0); // start point edge 0
            Halfedge h1p = mesh.prev_halfedge(h1); // start point edge 1

            // if start or end points differ more than seam_threshold
            // the corresponding edge is a texture seam
            if (norm(texcoords[h1] - texcoords[h0p]) > seam_threshold ||
                norm(texcoords[h0] - texcoords[h1p]) > seam_threshold)
            {
                seam_edges++;
                seams[e] = true;
            }
        }
    }

    // test loop case 2
    auto se = mesh.find_edge(Vertex(4), Vertex(11));
    // test loop case 1
    auto se2 = mesh.find_edge(Vertex(4), Vertex(0));

    EXPECT_EQ(mesh.n_vertices(), size_t(12));
    EXPECT_EQ(seam_edges, size_t(13));
    EXPECT_EQ(seams[se], 1);
    EXPECT_EQ(seams[se2], 1);
}
