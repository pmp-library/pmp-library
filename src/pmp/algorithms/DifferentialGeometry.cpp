//=============================================================================
// Copyright (C) 2011-2018 The pmp-library developers
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

#include <pmp/algorithms/DifferentialGeometry.h>

#include <limits>
#include <cmath>
#include <cfloat>

//=============================================================================

namespace pmp {

//=============================================================================

Scalar triangleArea(const Point& p0, const Point& p1, const Point& p2)
{
    return Scalar(0.5) * norm(cross(p1 - p0, p2 - p0));
}

//-----------------------------------------------------------------------------

Scalar triangleArea(const SurfaceMesh& mesh, SurfaceMesh::Face f)
{
    assert(mesh.valence(f) == 3);

    SurfaceMesh::VertexAroundFaceCirculator vfit = mesh.vertices(f);
    const Point&                            p0   = mesh.position(*vfit);
    ++vfit;
    const Point& p1 = mesh.position(*vfit);
    ++vfit;
    const Point& p2 = mesh.position(*vfit);

    return triangleArea(p0, p1, p2);
}

//-----------------------------------------------------------------------------

double cotanWeight(const SurfaceMesh& mesh, SurfaceMesh::Edge e)
{
    double weight = 0.0;

    const SurfaceMesh::Halfedge h0 = mesh.halfedge(e, 0);
    const SurfaceMesh::Halfedge h1 = mesh.halfedge(e, 1);

    const dvec3 p0 = (dvec3)mesh.position(mesh.toVertex(h0));
    const dvec3 p1 = (dvec3)mesh.position(mesh.toVertex(h1));

    if (!mesh.isSurfaceBoundary(h0))
    {
        const dvec3 p2 =
            (dvec3)mesh.position(mesh.toVertex(mesh.nextHalfedge(h0)));
        const dvec3  d0   = p0 - p2;
        const dvec3  d1   = p1 - p2;
        const double area = norm(cross(d0, d1));
        if (area > std::numeric_limits<double>::min())
        {
            const double cot = dot(d0, d1) / area;
            weight += clampCot(cot);
        }
    }

    if (!mesh.isSurfaceBoundary(h1))
    {
        const dvec3 p2 =
            (dvec3)mesh.position(mesh.toVertex(mesh.nextHalfedge(h1)));
        const dvec3  d0   = p0 - p2;
        const dvec3  d1   = p1 - p2;
        const double area = norm(cross(d0, d1));
        if (area > std::numeric_limits<double>::min())
        {
            const double cot = dot(d0, d1) / area;
            weight += clampCot(cot);
        }
    }

    assert(!std::isnan(weight));
    assert(!std::isinf(weight));

    return weight;
}

//-----------------------------------------------------------------------------

double voronoiArea(const SurfaceMesh& mesh, SurfaceMesh::Vertex v)
{
    double area(0.0);

    if (!mesh.isIsolated(v))
    {
        SurfaceMesh::Halfedge h0, h1, h2;
        dvec3                 P, Q, R, PQ, QR, PR;
        double                dotP, dotQ, dotR, triArea;
        double                cotQ, cotR;

        SurfaceMesh::HalfedgeAroundVertexCirculator vhit  = mesh.halfedges(v),
                                                    vhend = vhit;
        do
        {
            h0 = *vhit;
            h1 = mesh.nextHalfedge(h0);
            h2 = mesh.nextHalfedge(h1);

            if (mesh.isSurfaceBoundary(h0))
                continue;

            // three vertex positions
            P = (dvec3)mesh.position(mesh.toVertex(h2));
            Q = (dvec3)mesh.position(mesh.toVertex(h0));
            R = (dvec3)mesh.position(mesh.toVertex(h1));

            // edge vectors
            (PQ = Q) -= P;
            (QR = R) -= Q;
            (PR = R) -= P;

            // compute and check triangle area
            triArea = norm(cross(PQ, PR));
            if (triArea <= std::numeric_limits<double>::min())
                continue;

            // dot products for each corner (of its two emanating edge vectors)
            dotP = dot(PQ, PR);
            dotQ = -dot(QR, PQ);
            dotR = dot(QR, PR);

            // angle at P is obtuse
            if (dotP < 0.0)
            {
                area += 0.25 * triArea;
            }

            // angle at Q or R obtuse
            else if (dotQ < 0.0 || dotR < 0.0)
            {
                area += 0.125 * triArea;
            }

            // no obtuse angles
            else
            {
                // cot(angle) = cos(angle)/sin(angle) = dot(A,B)/norm(cross(A,B))
                cotQ = dotQ / triArea;
                cotR = dotR / triArea;

                // clamp cot(angle) by clamping angle to [1,179]
                area += 0.125 * (sqrnorm(PR) * clampCot(cotQ) +
                                 sqrnorm(PQ) * clampCot(cotR));
            }
        } while (++vhit != vhend);
    }

    assert(!std::isnan(area));
    assert(!std::isinf(area));

    return area;
}

//-----------------------------------------------------------------------------

double voronoiAreaBarycentric(const SurfaceMesh& mesh, SurfaceMesh::Vertex v)
{
    double area(0.0);

    if (!mesh.isIsolated(v))
    {
        const Point           P = mesh.position(v);
        SurfaceMesh::Halfedge h0, h1;
        Point                 Q, R, PQ, PR;

        SurfaceMesh::HalfedgeAroundVertexCirculator vhit  = mesh.halfedges(v),
                                                    vhend = vhit;
        do
        {
            if (mesh.isSurfaceBoundary(*vhit))
                continue;

            h0 = *vhit;
            h1 = mesh.nextHalfedge(h0);

            PQ = mesh.position(mesh.toVertex(h0));
            PQ -= P;

            PR = mesh.position(mesh.toVertex(h1));
            PR -= P;

            area += norm(cross(PQ, PR)) / 3.0;
        } while (++vhit != vhend);
    }

    return area;
}

//-----------------------------------------------------------------------------

Point laplace(const SurfaceMesh& mesh, SurfaceMesh::Vertex v)
{
    Point laplace(0.0, 0.0, 0.0);

    if (!mesh.isIsolated(v))
    {
        Scalar weight, sumWeights(0.0);

        SurfaceMesh::HalfedgeAroundVertexCirculator vhit  = mesh.halfedges(v),
                                                    vhend = vhit;
        do
        {
            weight = cotanWeight(mesh, mesh.edge(*vhit));
            sumWeights += weight;
            laplace += weight * mesh.position(mesh.toVertex(*vhit));
        } while (++vhit != vhend);

        laplace -= sumWeights * mesh.position(v);
        laplace /= Scalar(2.0) * voronoiArea(mesh, v);
    }

    return laplace;
}

//-----------------------------------------------------------------------------

Scalar angleSum(const SurfaceMesh& mesh, SurfaceMesh::Vertex v)
{
    Scalar angles(0.0);

    if (!mesh.isSurfaceBoundary(v))
    {
        const Point& p0 = mesh.position(v);

        SurfaceMesh::HalfedgeAroundVertexCirculator vhit  = mesh.halfedges(v),
                                                    vhend = vhit;
        do
        {
            const Point& p1 = mesh.position(mesh.toVertex(*vhit));
            const Point& p2 =
                mesh.position(mesh.toVertex(mesh.ccwRotatedHalfedge(*vhit)));

            const Point p01 = normalize(p1 - p0);
            const Point p02 = normalize(p2 - p0);

            Scalar cosAngle = clampCos(dot(p01, p02));

            angles += acos(cosAngle);
        } while (++vhit != vhend);
    }

    return angles;
}

//-----------------------------------------------------------------------------

VertexCurvature vertexCurvature(const SurfaceMesh& mesh, SurfaceMesh::Vertex v)
{
    VertexCurvature c;

    const Scalar area = voronoiArea(mesh, v);
    if (area > std::numeric_limits<Scalar>::min())
    {
        c.mean  = Scalar(0.5) * norm(laplace(mesh, v));
        c.gauss = (2.0 * M_PI - angleSum(mesh, v)) / voronoiArea(mesh, v);

        const Scalar s = sqrt(std::max(Scalar(0.0), c.mean * c.mean - c.gauss));
        c.min          = c.mean - s;
        c.max          = c.mean + s;

        assert(!std::isnan(c.mean));
        assert(!std::isnan(c.gauss));
        assert(!std::isinf(c.mean));
        assert(!std::isinf(c.gauss));

        assert(c.min <= c.mean);
        assert(c.mean <= c.max);
    }

    return c;
}

//=============================================================================
} // namespace pmp
//=============================================================================
