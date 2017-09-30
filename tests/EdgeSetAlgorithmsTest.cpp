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

#include <pmp/EdgeSet.h>
#include <pmp/algorithms/EdgeSetSmoothing.h>
#include <pmp/algorithms/EdgeSetSubdivision.h>

#include <vector>

using namespace pmp;

class EdgeSetAlgorithmsTest : public ::testing::Test
{
public:
    EdgeSetAlgorithmsTest()
    {
        es.read("pmp-data/knt/3rings.knt");
    }
    EdgeSet es;
};

TEST_F(EdgeSetAlgorithmsTest, smoothing)
{
    Scalar origBounds = es.bounds().size();
    EdgeSetSmoothing ess(es);
    ess.smooth(1);
    Scalar newBounds = es.bounds().size();
    EXPECT_LT(newBounds,origBounds);
}

TEST_F(EdgeSetAlgorithmsTest, subdivision)
{
    EdgeSetSubdivision esub(es);
    esub.subdivide();
    EXPECT_EQ(es.nVertices(),size_t(72));
}


//=============================================================================
