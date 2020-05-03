
// Copyright 2017-2019 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "gtest/gtest.h"

#include <pmp/algorithms/SurfaceSubdivision.h>
#include <pmp/algorithms/SurfaceFeatures.h>

using namespace pmp;

class SurfaceSubdivisionTest : public ::testing::Test
{
public:
    SurfaceSubdivisionTest()
    {
        EXPECT_TRUE(mesh.read("pmp-data/off/icosahedron_subdiv.off"));
    }
    SurfaceMesh mesh;
};

// plain loop subdivision
TEST_F(SurfaceSubdivisionTest, loop_subdivision)
{
    SurfaceSubdivision(mesh).loop();
    EXPECT_EQ(mesh.n_vertices(), size_t(2562));
}

// loop subdivision with features
TEST_F(SurfaceSubdivisionTest, loop_with_features)
{
    SurfaceFeatures sf(mesh);
    sf.detect_angle(25);

    SurfaceSubdivision(mesh).loop();
    EXPECT_EQ(mesh.n_vertices(), size_t(2562));
}

// loop subdivision with features
TEST_F(SurfaceSubdivisionTest, loop_with_boundary)
{
    mesh.clear();
    mesh.read("pmp-data/off/hemisphere.off");

    SurfaceSubdivision(mesh).loop();
    EXPECT_EQ(mesh.n_vertices(), size_t(7321));
}

// Catmull-Clark subdivision on suzanne quad mesh
TEST_F(SurfaceSubdivisionTest, catmull_clark_subdivision)
{
    mesh.clear();
    mesh.read("pmp-data/obj/suzanne.obj");
    SurfaceSubdivision(mesh).catmull_clark();
    EXPECT_EQ(mesh.n_vertices(), size_t(2012));
}

// Catmull-Clark subdivision on fandisk quad mesh
TEST_F(SurfaceSubdivisionTest, catmull_clark_with_features)
{
    mesh.clear();
    mesh.read("pmp-data/off/fandisk_quads.off");

    SurfaceFeatures sf(mesh);
    sf.detect_angle(25);

    SurfaceSubdivision(mesh).catmull_clark();
    EXPECT_EQ(mesh.n_vertices(), size_t(3058));
}

// plain sqrt3 subdivision
TEST_F(SurfaceSubdivisionTest, sqrt3Subdivision)
{
    SurfaceSubdivision(mesh).sqrt3();
    EXPECT_EQ(mesh.n_vertices(), size_t(1922));
}
