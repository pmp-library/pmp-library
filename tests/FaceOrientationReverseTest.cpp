// Copyright 2017-2020 the Polygon Mesh Processing Library developers.

// Distributed Distributed under a MIT-style license, see LICENSE.txt for details.

#include "gtest/gtest.h"

#include <pmp/algorithms/FaceOrientationReverse.h>
#include <pmp/algorithms/SurfaceNormals.h>

using namespace pmp;

class FaceOrientationReverseTest : public ::testing::Test
{
public:
    FaceOrientationReverseTest()
    {
        EXPECT_TRUE(mesh.read("pmp-data/off/hemisphere.off"));
    }
    SurfaceMesh mesh;
};

TEST_F(FaceOrientationReverseTest, reverse)
{
    SurfaceNormals::compute_face_normals(mesh);

    auto prenormal = mesh.add_face_property<Normal>("f:prenormal");
    auto normal = mesh.get_face_property<Normal>("f:normal");
    for (auto f : mesh.faces())
    {
        prenormal[f] = normal[f];
    }

    FaceOrientationReverse reverse(mesh);
    reverse.reverse();

    SurfaceNormals::compute_face_normals(mesh);

    for (auto f : mesh.faces())
    {
        EXPECT_FLOAT_EQ(prenormal[f][0], -normal[f][0]);
        EXPECT_FLOAT_EQ(prenormal[f][1], -normal[f][1]);
        EXPECT_FLOAT_EQ(prenormal[f][2], -normal[f][2]);
    }
}

