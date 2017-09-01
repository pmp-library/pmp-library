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

#include <surface_mesh/PointSet.h>
#include <surface_mesh/io/PointSetIO.h>
#include <vector>

using namespace surface_mesh;

class PointSetTest : public ::testing::Test
{
public:
    PointSet ps;
    Point p0{0,0,0};
    Point p1{1,1,1};
};

// test with new in order to catch all created functions
TEST_F(PointSetTest, instantiate)
{
    PointSet* ps = new PointSet();
    EXPECT_EQ(ps->nVertices(), size_t(0));
}

TEST_F(PointSetTest, operatorEq)
{
    ps.addVertex(p0);
    ps.addVertex(p1);
    PointSet ps2 = ps;
    EXPECT_EQ(ps2.nVertices(), size_t(2));
}

TEST_F(PointSetTest, assignment)
{
    ps.addVertex(p0);
    ps.addVertex(p1);
    PointSet ps2;
    ps2.assign(ps);
    EXPECT_EQ(ps2.nVertices(), size_t(2));
}

TEST_F(PointSetTest, addRemove)
{
    PointSet ps;
    ps.reserve(2);
    auto  v1 = ps.addVertex(p0);
    ps.addVertex(p1);
    EXPECT_EQ(ps.nVertices(), size_t(2));
    EXPECT_EQ(ps.pointVector().size(), size_t(2));
    ps.deleteVertex(v1);
    ps.garbageCollection();
    EXPECT_EQ(ps.nVertices(), size_t(1));
    EXPECT_EQ(ps.pointVector().size(), size_t(1));
    ps.clear();
    EXPECT_EQ(ps.nVertices(), size_t(0));
    EXPECT_EQ(ps.pointVector().size(), size_t(0));
}

TEST_F(PointSetTest, iterators)
{
    size_t   nv(0);
    ps.addVertex(p0);
    ps.addVertex(p1);
    for (auto v : ps.vertices())
    {
        SM_ASSERT(v.isValid());
        nv++;
    }
    EXPECT_EQ(nv, size_t(2));
}

TEST_F(PointSetTest, skipDeleted)
{
    size_t   nv(0);
    auto     v1 = ps.addVertex(p0);
    ps.addVertex(p1);
    ps.deleteVertex(v1);
    for (auto v : ps.vertices())
    {
        SM_ASSERT(v.isValid());
        nv++;
    }
    EXPECT_EQ(nv, size_t(1));
}

TEST_F(PointSetTest, vertexProperties)
{
    // explicit add
    auto v0 = ps.addVertex(p1);
    auto osize = ps.vertexProperties().size();
    auto vidx = ps.addVertexProperty<int>("v:idx");
    EXPECT_EQ(ps.vertexProperties().size(), osize+1);
    vidx[v0] = 0;

    ps.removeVertexProperty(vidx);
    EXPECT_EQ(ps.vertexProperties().size(), osize);

    // implicit add
    vidx = ps.vertexProperty<int>("v:idx2");
    EXPECT_EQ(ps.vertexProperties().size(), osize+1);
    ps.removeVertexProperty(vidx);
    EXPECT_EQ(ps.vertexProperties().size(), osize);
}

TEST_F(PointSetTest, write)
{
    ps.addVertex(p0);
    ps.addVertex(p1);
    ps.write("test.xyz");
    ps.clear();
    EXPECT_EQ(ps.nVertices(), size_t(0));
}

TEST_F(PointSetTest, read)
{
    ps.read("test.xyz"); // bad test dependency
    EXPECT_EQ(ps.nVertices(), size_t(2));
}

TEST_F(PointSetTest, readFailure)
{
    ASSERT_FALSE(ps.read("test.off"));
}
