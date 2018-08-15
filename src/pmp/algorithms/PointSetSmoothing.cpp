//=============================================================================
// Copyright (C) 2011-2017 The pmp-library developers
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
//=============================================================================

#include <pmp/algorithms/PointSetSmoothing.h>
#include <pmp/algorithms/PointKdTree.h>

#include <limits>

//=============================================================================

namespace pmp {

//=============================================================================

PointSetSmoothing::PointSetSmoothing(PointSet& pointSet) : m_pointSet(pointSet)
{
    // check if normals are present
    if (!m_pointSet.getVertexProperty<Normal>("v:normal"))
        std::cerr << "PointSetSmoothing: Normals required!" << std::endl;

    // Parameter settings for MLS
    m_radius = 0.04 * m_pointSet.bounds().size();
    m_epsilon = 1e-4;
    m_maxIter = 3;
}

//-----------------------------------------------------------------------------

void PointSetSmoothing::smooth()
{
    PointKdTree tree(m_pointSet);
    tree.build(10, 99);

    PointSet projected = m_pointSet;

    auto vnormals = m_pointSet.getVertexProperty<Normal>("v:normal");

    if (!vnormals)
        std::cerr << "PointSetSmoothing: Normals required!" << std::endl;

    auto ppoints = projected.getVertexProperty<Point>("v:point");
    auto pnormals = projected.vertexProperty<Normal>("v:normal");

    for (auto v : m_pointSet.vertices())
    {
        Point x = m_pointSet.position(v);
        Point n = vnormals[v];

        project(x, n, tree);

        ppoints[v] = x;
        pnormals[v] = n;
    }

    m_pointSet = projected;
}

//-----------------------------------------------------------------------------

void PointSetSmoothing::project(Point& x, Normal& n,
                                const PointKdTree& tree) const
{
    // Get ball
    std::vector<int> ball;
    tree.ball(x, m_radius, ball);

    // Project point with position \c x and normal \c n to MLS surface: Uses \c
    // tree.ball() for a ball query. Iterates until either the projection error
    // is below \c m_epsilon or the maximum number of iterations \c m_maxIter is
    // reached.

    // Weighted average
    x = weightedAveragePosition(x, ball);

    Scalar f = std::numeric_limits<Scalar>::max();
    size_t step = 0;
    Point a;
    while (true)
    {
        ++step;

        // get ball
        tree.ball(x, m_radius, ball);

        // averaged normal
        n = weightedAverageNormal(x, ball);

        // averaged position
        a = weightedAveragePosition(x, ball);

        // projection
        f = dot(n, a - x);

        // check for termination
        if (fabs(f) < m_epsilon || step > m_maxIter)
            break;
        else
            x += n * f;
    };
}

//-----------------------------------------------------------------------------

Point PointSetSmoothing::weightedAveragePosition(const Point& x,
                                                 std::vector<int>& ball) const
{
    assert(ball.size() > 0);

    // Positions of point set
    auto points = m_pointSet.getVertexProperty<Point>("v:point");

    // Compute the weighted average of the positions from points with indices in
    // \c ball to point \c x. Uses theta() as weight function.
    Point num(0.0);
    Scalar denom(0.0);
    for (auto b : ball)
    {
        PointSet::Vertex v(b);
        Scalar dist = norm(x - points[v]);
        num += theta(dist) * points[v];
        denom += theta(dist);
    }

    return num / denom;
}

//-----------------------------------------------------------------------------

Point PointSetSmoothing::weightedAverageNormal(const Point& x,
                                               std::vector<int>& ball) const
{
    assert(ball.size() > 0);

    // Positions and normals of point set
    auto points = m_pointSet.getVertexProperty<Point>("v:point");
    auto normals = m_pointSet.getVertexProperty<Normal>("v:normal");

    // Compute weighted average of the normals from points with indices
    // in \c ball to point \c x. Uses theta() as weight function.
    Normal n(0.0);
    for (auto b : ball)
    {
        PointSet::Vertex v(b);
        Scalar dist = norm(x - points[v]);
        n += theta(dist) * normals[v];
    }
    return normalize(n);
}

//=============================================================================
} // namespace pmp
//=============================================================================
