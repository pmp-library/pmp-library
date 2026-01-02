// Copyright 2017-2019 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#include "gtest/gtest.h"

#include "pmp/algorithms/curvature.h"
#include "pmp/algorithms/shapes.h"

using namespace pmp;

class CurvatureTest : public ::testing::Test
{
public:
    CurvatureTest() { mesh = icosphere(5); }

    SurfaceMesh mesh;
};

TEST_F(CurvatureTest, curvature)
{
    Scalar kmin = std::numeric_limits<Scalar>::max();
    Scalar kmax = -std::numeric_limits<Scalar>::max();
    Scalar mmin = std::numeric_limits<Scalar>::max();
    Scalar mmax = -std::numeric_limits<Scalar>::max();
    Scalar gmin = std::numeric_limits<Scalar>::max();
    Scalar gmax = -std::numeric_limits<Scalar>::max();

    curvature(mesh, Curvature::Min, 1);
    auto vcurv = mesh.vertex_property<Scalar>("v:curv");
    for (auto v : mesh.vertices())
        kmin = std::min(kmin, vcurv[v]);

    curvature(mesh, Curvature::Max, 1);
    for (auto v : mesh.vertices())
        kmax = std::max(kmax, vcurv[v]);

    curvature(mesh, Curvature::Mean, 1);
    for (auto v : mesh.vertices())
    {
        mmin = std::min(mmin, vcurv[v]);
        mmax = std::max(mmax, vcurv[v]);
    }

    curvature(mesh, Curvature::Gauss, 1);
    for (auto v : mesh.vertices())
    {
        gmin = std::min(gmin, vcurv[v]);
        gmax = std::max(gmax, vcurv[v]);
    }

    EXPECT_NEAR(kmin, 1.0, 0.02);
    EXPECT_NEAR(kmax, 1.0, 0.02);
    EXPECT_NEAR(mmin, 1.0, 0.02);
    EXPECT_NEAR(mmax, 1.0, 0.02);
    EXPECT_NEAR(gmin, 1.0, 0.02);
    EXPECT_NEAR(gmax, 1.0, 0.02);
}

TEST_F(CurvatureTest, curvature_to_texture_coordinates)
{
    curvature(mesh, Curvature::Mean, 1);
    curvature_to_texture_coordinates(mesh);
    auto tex = mesh.vertex_property<TexCoord>("v:tex");
    EXPECT_TRUE(tex);
}