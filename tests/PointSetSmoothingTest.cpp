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

#include <pmp/PointSet.h>
#include <pmp/algorithms/PointSetSmoothing.h>

#include <random>

using namespace pmp;

// Randomly generate nPoints on the unit sphere.
// Stores the resulting points and normals in ps.
void generateRandomSphere(size_t nPoints, PointSet& ps)
{
    ps.clear();

    // normal distribution random number generator
    std::default_random_engine generator;
    std::normal_distribution<double> distribution(0.0, 1.0);

    auto normals = ps.vertexProperty<Normal>("v:normal");

    // generate points and add to point set
    for (size_t i(0); i < nPoints; i++)
    {
        double x = distribution(generator);
        double y = distribution(generator);
        double z = distribution(generator);

        // reject if all zero
        if (x == 0 && y == 0 && z == 0)
        {
            i--;
            continue;
        }

        // normalize
        double mult = 1.0 / sqrt(x * x + y * y + z * z);
        Point p(x, y, z);
        p *= mult;

        // add point and normal
        auto v = ps.addVertex(p);
        normals[v] = p;
    }
}

TEST(PointSetSmoothingTest, smoothRandomSphere)
{
    PointSet ps;
    generateRandomSphere(1000, ps);
    Scalar origBounds = ps.bounds().size();

    PointSetSmoothing pss(ps);
    pss.smooth();

    Scalar newBounds = ps.bounds().size();
    EXPECT_LT(newBounds, origBounds);
}
