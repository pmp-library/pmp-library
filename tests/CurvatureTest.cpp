// Copyright 2017-2019 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "gtest/gtest.h"

#include "pmp/algorithms/Curvature.h"
#include "pmp/algorithms/Shapes.h"

using namespace pmp;

class CurvatureTest : public ::testing::Test
{
public:
    CurvatureTest()
    {
        mesh = Shapes::icosphere(5);
        curvature = new Curvature(mesh);
        curvature->analyze(1);
    }

    ~CurvatureTest() { delete curvature; }

    SurfaceMesh mesh;
    Curvature* curvature;
};

TEST_F(CurvatureTest, curvature)
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

    EXPECT_NEAR(kmin, 1.0, 0.02);
    EXPECT_NEAR(kmax, 1.0, 0.02);
    EXPECT_NEAR(mmin, 1.0, 0.02);
    EXPECT_NEAR(mmax, 1.0, 0.02);
    EXPECT_NEAR(gmin, 1.0, 0.02);
    EXPECT_NEAR(gmax, 1.0, 0.02);
}

TEST_F(CurvatureTest, mean_curvature_to_texture_coordinates)
{
    curvature->mean_curvature_to_texture_coordinates();
    auto tex = mesh.vertex_property<TexCoord>("v:tex");
    EXPECT_TRUE(tex);
}

TEST_F(CurvatureTest, max_curvature_to_texture_coordinates)
{
    curvature->max_curvature_to_texture_coordinates();
    auto tex = mesh.vertex_property<TexCoord>("v:tex");
    EXPECT_TRUE(tex);
}

TEST_F(CurvatureTest, gauss_curvature_to_texture_coordinates)
{
    curvature->gauss_curvature_to_texture_coordinates();
    auto tex = mesh.vertex_property<TexCoord>("v:tex");
    EXPECT_TRUE(tex);
}
