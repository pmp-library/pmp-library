//=============================================================================
// Copyright (C) 2017-2019 The pmp-library developers
//
// This file is part of the Polygon Mesh Processing Library.
// Distributed under a MIT-style license, see LICENSE.txt for details.
//
// SPDX-License-Identifier: MIT-with-employer-disclaimer
//=============================================================================

#include "gtest/gtest.h"

#include <pmp/SurfaceMesh.h>
#include <pmp/MatVec.h>
#include <vector>

using namespace pmp;

class EigenTest: public ::testing::Test
{
public:

};

TEST_F(EigenTest, construct_from_eigen)
{
    {
        Eigen::Vector2f eigenVec(1.0f, 2.0f);
        vec2 pmpVec = eigenVec;
        EXPECT_EQ(pmpVec[1], 2.0f);
    }
    {
        Eigen::Vector3d eigenVec(1.0, 2.0, 3.0);
        dvec3 pmpVec = eigenVec;
        EXPECT_EQ(pmpVec[1], 2.0);
    }
    {
        Eigen::Vector4i eigenVec(1, 2, 3, 4);
        ivec4 pmpVec = eigenVec;
        EXPECT_EQ(pmpVec[1], 2);
    }
}

TEST_F(EigenTest, assignment_from_eigen)
{
    {
        Eigen::Vector2f eigenVec(1.0f, 2.0f);
        vec2 pmpVec;
        pmpVec = eigenVec;
        EXPECT_EQ(pmpVec[1], 2.0f);
    }
    {
        Eigen::Vector3d eigenVec(1.0, 2.0, 3.0);
        dvec3 pmpVec;
        pmpVec = eigenVec;
        EXPECT_EQ(pmpVec[1], 2.0);
    }
    {
        Eigen::Vector4i eigenVec(1, 2, 3, 4);
        ivec4 pmpVec;
        pmpVec = eigenVec;
        EXPECT_EQ(pmpVec[1], 2);
    }
}

TEST_F(EigenTest, cast_to_eigen)
{
    {
        vec2 pmpVec(1.0f, 2.0f);
        Eigen::Vector2f eigenVec = static_cast<Eigen::Vector2f>(pmpVec);
        EXPECT_EQ(eigenVec[1], 2.0f);
    }
    {
        dvec3 pmpVec(1.0, 2.0, 3.0);
        Eigen::Vector3f eigenVec = static_cast<Eigen::Vector3f>(pmpVec);
        EXPECT_EQ(eigenVec[1], 2.0);
    }
    {
        ivec4 pmpVec(1, 2, 3, 4);
        Eigen::Vector4i eigenVec = static_cast<Eigen::Vector4i>(pmpVec);
        EXPECT_EQ(eigenVec[1], 2);
    }
}

