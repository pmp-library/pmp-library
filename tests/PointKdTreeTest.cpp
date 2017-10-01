//=============================================================================
// Copyright (C) 2017 The pmp-library developers
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
#include <pmp/algorithms/PointKdTree.h>

#include <vector>

using namespace pmp;

class PointKdTreeTest : public ::testing::Test
{
public:
    PointKdTreeTest()
    {
        ps.read("pmp-data/xyz/armadillo_low.xyz");
        tree = new PointKdTree(ps);
        tree->build(10, 99);

    }
    ~PointKdTreeTest()
    {
        delete tree;
    }

    PointSet ps;
    PointKdTree* tree;
};

TEST_F(PointKdTreeTest, nearest)
{
    int idx(0);
    Point p(0,0,0);
    Point nearest;
    tree->nearest(p,nearest,idx);
    EXPECT_GT(norm(nearest),0);
    EXPECT_GT(idx,0);

    // compare against exhaustive search
    double dist(0);
    double dmin(std::numeric_limits<double>::max());
    PointSet::Vertex vmin;
    for (auto v : ps.vertices())
    {
        dist = distance(p,ps.position(v));
        if (dist < dmin)
        {
            dmin = dist;
            vmin = v;
        }
    }
    EXPECT_EQ(nearest,ps.position(vmin));
    EXPECT_EQ(idx,vmin.idx());
}

TEST_F(PointKdTreeTest, kNearest)
{
    std::vector<int> knn;
    int k = 6;
    Point p(0,0,0);

    tree->kNearest(p,k,knn);
    for (auto idx : knn)
    {
        EXPECT_GT(idx,0);
    }

    // compare against exhaustive search
    typedef std::pair<PointSet::Vertex,float> Elem;
    std::vector<Elem> distances;

    for (auto v : ps.vertices())
    {
        auto d = distance(p,ps.position(v));
        distances.push_back(std::make_pair(v,d));
    }

    // sort based on distance
    std::sort(distances.begin(), distances.end(), [](Elem &left, Elem &right) {
        return left.second < right.second;
    });

    // check indices for equality
    size_t i(0);
    for (auto idx : knn)
    {
        EXPECT_EQ(idx,distances[i++].first.idx());
    }
}
