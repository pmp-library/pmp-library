
// Copyright 2017-2019 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "gtest/gtest.h"

#include <pmp/algorithms/SurfaceCurvature.h>

using namespace pmp;

class SurfaceCurvatureTest : public ::testing::Test
{
public:
    SurfaceCurvatureTest()
    {
        EXPECT_TRUE(mesh.read("pmp-data/off/hemisphere.off"));
        curvature = new SurfaceCurvature(mesh);
        curvature->analyze(1);
    }

    ~SurfaceCurvatureTest() { delete curvature; }

    SurfaceMesh mesh;
    SurfaceCurvature* curvature;
};

TEST_F(SurfaceCurvatureTest, curvature)
{
    Scalar kmin = std::numeric_limits<Scalar>::max();
    Scalar kmax = -std::numeric_limits<Scalar>::max();
    Scalar mmin = std::numeric_limits<Scalar>::max();
    Scalar mmax = -std::numeric_limits<Scalar>::max();
    Scalar gmin = std::numeric_limits<Scalar>::max();
    Scalar gmax = -std::numeric_limits<Scalar>::max();

    for (auto v : mesh.vertices())
    {
        kmin = std::min(kmin, curvature->min_curvature(v));
        kmax = std::max(kmax, curvature->max_curvature(v));
        mmin = std::min(mmin, curvature->mean_curvature(v));
        mmax = std::max(mmax, curvature->mean_curvature(v));
        gmin = std::min(gmin, curvature->gauss_curvature(v));
        gmax = std::max(gmax, curvature->gauss_curvature(v));
    }

#ifdef PMP_SCALAR_TYPE_64
    EXPECT_FLOAT_EQ(kmin, 0.50240165);
    EXPECT_FLOAT_EQ(kmax, 1.0002906);
    EXPECT_FLOAT_EQ(mmin, 0.50240165);
    EXPECT_FLOAT_EQ(mmax, 1.0002906);
    EXPECT_FLOAT_EQ(gmin, 0.2524074);
    EXPECT_FLOAT_EQ(gmax, 1.0005813);
#else
    EXPECT_FLOAT_EQ(kmin, 0.50240648);
    EXPECT_FLOAT_EQ(kmax, 1.0003014);
    EXPECT_FLOAT_EQ(mmin, 0.50240648);
    EXPECT_FLOAT_EQ(mmax, 1.0003014);
    EXPECT_FLOAT_EQ(gmin, 0.25241226);
    EXPECT_FLOAT_EQ(gmax, 1.0006028);
#endif
}

TEST_F(SurfaceCurvatureTest, mean_curvature_to_texture_coordinates)
{
    curvature->mean_curvature_to_texture_coordinates();
    auto tex = mesh.vertex_property<TexCoord>("v:tex");
    EXPECT_TRUE(tex);
}

TEST_F(SurfaceCurvatureTest, max_curvature_to_texture_coordinates)
{
    curvature->max_curvature_to_texture_coordinates();
    auto tex = mesh.vertex_property<TexCoord>("v:tex");
    EXPECT_TRUE(tex);
}

TEST_F(SurfaceCurvatureTest, gauss_curvature_to_texture_coordinates)
{
    curvature->gauss_curvature_to_texture_coordinates();
    auto tex = mesh.vertex_property<TexCoord>("v:tex");
    EXPECT_TRUE(tex);
}
