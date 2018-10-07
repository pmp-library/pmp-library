//=============================================================================
// Copyright (C) 2017, 2018 The pmp-library developers
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
// =============================================================================

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

    ~SurfaceCurvatureTest()
    {
        delete curvature;
    }

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
        kmin = std::min(kmin, curvature->minCurvature(v));
        kmax = std::max(kmax, curvature->maxCurvature(v));
        mmin = std::min(mmin, curvature->meanCurvature(v));
        mmax = std::max(mmax, curvature->meanCurvature(v));
        gmin = std::min(gmin, curvature->gaussCurvature(v));
        gmax = std::max(gmax, curvature->gaussCurvature(v));
    }

    EXPECT_FLOAT_EQ(kmin,0.50240648);
    EXPECT_FLOAT_EQ(kmax,1.0003014);
    EXPECT_FLOAT_EQ(mmin,0.50240648);
    EXPECT_FLOAT_EQ(mmax,1.0003014);
    EXPECT_FLOAT_EQ(gmin,0.25241226);
    EXPECT_FLOAT_EQ(gmax,1.0006028);
}

TEST_F(SurfaceCurvatureTest, meanCurvatureToTextureCoordinates)
{
    curvature->meanCurvatureToTextureCoordinates();
    auto tex = mesh.vertexProperty<TextureCoordinate>("v:tex");
    EXPECT_TRUE(tex);
}

TEST_F(SurfaceCurvatureTest, maxCurvatureToTextureCoordinates)
{
    curvature->maxCurvatureToTextureCoordinates();
    auto tex = mesh.vertexProperty<TextureCoordinate>("v:tex");
    EXPECT_TRUE(tex);
}

TEST_F(SurfaceCurvatureTest, gaussCurvatureToTextureCoordinates)
{
    curvature->gaussCurvatureToTextureCoordinates();
    auto tex = mesh.vertexProperty<TextureCoordinate>("v:tex");
    EXPECT_TRUE(tex);
}
