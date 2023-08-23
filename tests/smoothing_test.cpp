// Copyright 2017-2021 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "gtest/gtest.h"

#include "pmp/algorithms/smoothing.h"
#include "pmp/algorithms/differential_geometry.h"
#include "helpers.h"

using namespace pmp;

TEST(SmoothingTest, implicit_smoothing)
{
    auto mesh = open_cone();
    auto area_before = surface_area(mesh);
    implicit_smoothing(mesh, 0.01, 1.0, false, false);
    auto area_after = surface_area(mesh);
    EXPECT_LT(area_after, area_before);
}

TEST(SmoothingTest, implicit_smoothing_with_rescale)
{
    auto mesh = open_cone();
    auto area_before = surface_area(mesh);
    implicit_smoothing(mesh, 0.01, 1.0, false, true);
    auto area_after = surface_area(mesh);
    EXPECT_FLOAT_EQ(area_after, area_before);
}

TEST(SmoothingTest, explicit_smoothing)
{
    auto mesh = open_cone();
    auto area_before = surface_area(mesh);
    explicit_smoothing(mesh, 10, true);
    explicit_smoothing(mesh, 10, false);
    auto area_after = surface_area(mesh);
    EXPECT_LT(area_after, area_before);
}
