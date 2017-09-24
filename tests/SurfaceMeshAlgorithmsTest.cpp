//=============================================================================
// Copyright (C) 2017 Daniel Sieger
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

#include <surface_mesh/SurfaceMesh.h>
#include <surface_mesh/algorithms/SurfaceSimplification.h>
#include <surface_mesh/algorithms/SurfaceFeatures.h>
#include <surface_mesh/algorithms/SurfaceSubdivision.h>
#include <surface_mesh/algorithms/SurfaceRemeshing.h>
#include <surface_mesh/algorithms/SurfaceCurvature.h>

#include <vector>

using namespace surface_mesh;

class SurfaceMeshAlgorithmsTest : public ::testing::Test
{
public:
    SurfaceMeshAlgorithmsTest()
    {
        EXPECT_TRUE(mesh.read("pmp-data/off/icosahedron_subdiv.off"));
    }
    SurfaceMesh mesh;
};

// curvature
TEST_F(SurfaceMeshAlgorithmsTest, curvature)
{
    mesh.clear();
    mesh.read("pmp-data/off/hemisphere.off");
    SurfaceCurvature curvature(mesh);
    curvature.analyze(1);
    Scalar kmin = std::numeric_limits<Scalar>::max();
    Scalar kmax = std::numeric_limits<Scalar>::min();
    for (auto v : mesh.vertices())
    {
        kmin = std::min(kmin, curvature.minCurvature(v));
        kmax = std::max(kmax, curvature.maxCurvature(v));
    }
    EXPECT_FLOAT_EQ(kmin,0.50240648);
    EXPECT_FLOAT_EQ(kmax,1.0003014);
}

// feature angle
TEST_F(SurfaceMeshAlgorithmsTest, featureAngle)
{
    SurfaceFeatures sf(mesh);
    sf.detectAngle(25);

    auto efeature = mesh.getEdgeProperty<bool>("e:feature");
    bool found = false;
    for (auto e : mesh.edges())
        if (efeature[e])
        {
            found = true;
            break;
        }
    EXPECT_TRUE(found);
    sf.clear();
    found = false;
    for (auto e : mesh.edges())
        if (efeature[e])
        {
            found = true;
            break;
        }
    EXPECT_FALSE(found);
}

// boundary edges
TEST_F(SurfaceMeshAlgorithmsTest, featureBoundary)
{
    mesh.clear();
    mesh.read("data/vertex_onering.off");
    SurfaceFeatures sf(mesh);
    sf.detectBoundary();

    auto efeature = mesh.getEdgeProperty<bool>("e:feature");
    bool found = false;
    for (auto e : mesh.edges())
        if (efeature[e])
        {
            found = true;
            break;
        }
    EXPECT_TRUE(found);
}

// plain simplification test
TEST_F(SurfaceMeshAlgorithmsTest, simplification)
{
    SurfaceSimplification ss(mesh);
    ss.initialize(5); // aspect ratio
    ss.simplify(mesh.nVertices() * 0.1);
    EXPECT_EQ(mesh.nVertices(),size_t(64));
}

// simplify with feature edge preservation enabled
TEST_F(SurfaceMeshAlgorithmsTest, simplificationWithFeatures)
{
    SurfaceFeatures sf(mesh);
    sf.detectAngle(25);

    SurfaceSimplification ss(mesh);
    ss.initialize(5); // aspect ratio
    ss.simplify(mesh.nVertices() * 0.1);
    EXPECT_EQ(mesh.nVertices(),size_t(64));
}

// plain loop subdivision
TEST_F(SurfaceMeshAlgorithmsTest, loopSubdivision)
{
    SurfaceSubdivision(mesh).loop();
    EXPECT_EQ(mesh.nVertices(),size_t(2562));
}

// Catmull-Clark subdivision on suzanne quad mesh
TEST_F(SurfaceMeshAlgorithmsTest, catmullClarkSubdivision)
{
    mesh.clear();
    mesh.read("pmp-data/obj/suzanne.obj");
    SurfaceSubdivision(mesh).catmullClark();
    EXPECT_EQ(mesh.nVertices(),size_t(2012));
}

// plain sqrt3 subdivision
TEST_F(SurfaceMeshAlgorithmsTest, sqrt3Subdivision)
{
    SurfaceSubdivision(mesh).sqrt3();
    EXPECT_EQ(mesh.nVertices(),size_t(1922));
}

// adaptive remeshing
TEST_F(SurfaceMeshAlgorithmsTest, adaptiveRemeshing)
{
    // adaptive remeshing
    SurfaceSubdivision(mesh).loop();
    auto bb = mesh.bounds().size();
    SurfaceRemeshing(mesh).adaptiveRemeshing(
        0.001 * bb,  // min length
        1.0 * bb,    // max length
        0.001 * bb); // approx. error
    EXPECT_EQ(mesh.nVertices(),size_t(2142));
}

TEST_F(SurfaceMeshAlgorithmsTest, uniformRemeshing)
{
    Scalar l(0);
    for (auto eit : mesh.edges())
        l += distance(mesh.position(mesh.vertex(eit, 0)),
                      mesh.position(mesh.vertex(eit, 1)));
    l /= (Scalar)mesh.nEdges();
    SurfaceRemeshing(mesh).uniformRemeshing(l);
    EXPECT_EQ(mesh.nVertices(),size_t(642));
}
