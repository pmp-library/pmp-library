//=============================================================================
// Copyright (C) 2017-2018 The pmp-library developers
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

#include "PointSetTest.h"

#include <pmp/io/PointSetIO.h>

using namespace pmp;

class PointSetIOTest : public PointSetTest {};

TEST_F(PointSetIOTest, write)
{
    ps.addVertex(p0);
    ps.addVertex(p1);
    ps.write("test.xyz");
    ps.clear();
    EXPECT_EQ(ps.nVertices(), size_t(0));

    // check malformed file names
    EXPECT_FALSE(ps.write("testxxyyzz"));
}

TEST_F(PointSetIOTest, readXYZ)
{
    ps.read("test.xyz"); // bad test dependency
    EXPECT_EQ(ps.nVertices(), size_t(2));
}

TEST_F(PointSetIOTest, readAGI)
{
    ps.read("pmp-data/agi/test.agi");
    EXPECT_EQ(ps.nVertices(), size_t(4));
    auto vcolors = ps.getVertexProperty<Color>("v:color");
    EXPECT_TRUE(vcolors);
    auto vnormals = ps.getVertexProperty<Normal>("v:normal");
    EXPECT_TRUE(vnormals);
}

TEST_F(PointSetIOTest, readFailure)
{
    ASSERT_FALSE(ps.read("test.off"));
}