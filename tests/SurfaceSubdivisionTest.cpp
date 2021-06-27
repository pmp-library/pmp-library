// Copyright 2017-2021 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "gtest/gtest.h"

#include "pmp/algorithms/SurfaceSubdivision.h"
#include "pmp/algorithms/SurfaceFeatures.h"
#include "Helpers.h"

using namespace pmp;

// plain loop subdivision
TEST(SurfaceSubdivisionTest, loop_subdivision)
{
    auto mesh = subdivided_icosahedron();
    SurfaceFeatures(mesh).clear();
    SurfaceSubdivision(mesh).loop();
    EXPECT_EQ(mesh.n_vertices(), size_t(2562));
}

// loop subdivision with features
TEST(SurfaceSubdivisionTest, loop_with_features)
{
    auto mesh = subdivided_icosahedron();

    SurfaceFeatures sf(mesh);
    sf.detect_angle(25);

    SurfaceSubdivision(mesh).loop();
    EXPECT_EQ(mesh.n_vertices(), size_t(2562));
}

// loop subdivision with features
TEST(SurfaceSubdivisionTest, loop_with_boundary)
{
    auto mesh = hemisphere();
    SurfaceSubdivision(mesh).loop();
    EXPECT_EQ(mesh.n_vertices(), size_t(3597));
}

// Catmull-Clark subdivision on suzanne quad mesh
TEST(SurfaceSubdivisionTest, catmull_clark_subdivision)
{
    SurfaceMesh mesh;
    mesh.read("pmp-data/obj/suzanne.obj");
    SurfaceSubdivision(mesh).catmull_clark();
    EXPECT_EQ(mesh.n_vertices(), size_t(2012));
}

// Catmull-Clark subdivision on fandisk quad mesh
TEST(SurfaceSubdivisionTest, catmull_clark_with_features)
{
    SurfaceMesh mesh;
    mesh.read("pmp-data/off/fandisk_quads.off");

    SurfaceFeatures sf(mesh);
    sf.detect_angle(25);

    SurfaceSubdivision(mesh).catmull_clark();
    EXPECT_EQ(mesh.n_vertices(), size_t(3058));
}

// plain sqrt3 subdivision
TEST(SurfaceSubdivisionTest, sqrt3Subdivision)
{
    auto mesh = subdivided_icosahedron();
    SurfaceFeatures(mesh).clear();
    SurfaceSubdivision(mesh).sqrt3();
    EXPECT_EQ(mesh.n_vertices(), size_t(1922));
}
