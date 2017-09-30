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

#include <pmp/algorithms/distancePointTriangle.h>
#include <cfloat>
#include <cmath>

//=============================================================================

namespace pmp {

//=============================================================================

Scalar distPointLineSegment(const Point& P, const Point& V0, const Point& V1,
                            Point& NearestPoint)
{
    Point  d1(P - V0);
    Point  d2(V1 - V0);
    Point  minV(V0);
    Scalar t = dot(d2, d2);

    if (t > FLT_MIN)
    {
        t = dot(d1, d2) / t;
        if (t > 1.0)
            d1 = P - (minV = V1);
        else if (t > 0.0)
            d1 = P - (minV = V0 + d2 * t);
    }

    NearestPoint = minV;
    return norm(d1);
}

//-----------------------------------------------------------------------------

Scalar distPointTriangle(const Point& P, const Point& V0, const Point& V1,
                         const Point& V2, Point& NearestPoint)
{
    Point  v0v1 = V1 - V0;
    Point  v0v2 = V2 - V0;
    Point  n    = cross(v0v1, v0v2); // not normalized !
    Scalar d    = sqrnorm(n);

    // Check if the triangle is degenerated -> measure dist to line segments
    if (fabs(d) < FLT_MIN)
    {
        Point  q, qq;
        Scalar d, dd(FLT_MAX);

        dd = distPointLineSegment(P, V0, V1, qq);

        d = distPointLineSegment(P, V1, V2, q);
        if (d < dd)
        {
            dd = d;
            qq = q;
        }

        d = distPointLineSegment(P, V2, V0, q);
        if (d < dd)
        {
            dd = d;
            qq = q;
        }

        NearestPoint = qq;
        return dd;
    }

    Scalar invD = 1.0 / d;
    Point  v1v2 = V2;
    v1v2 -= V1;
    Point v0p = P;
    v0p -= V0;
    Point  t = cross(v0p, n);
    Scalar a = dot(t, v0v2) * -invD;
    Scalar b = dot(t, v0v1) * invD;
    Scalar s01, s02, s12;

    // Calculate the distance to an edge or a corner vertex
    if (a < 0)
    {
        s02 = dot(v0v2, v0p) / sqrnorm(v0v2);
        if (s02 < 0.0)
        {
            s01 = dot(v0v1, v0p) / sqrnorm(v0v1);
            if (s01 <= 0.0)
            {
                v0p = V0;
            }
            else if (s01 >= 1.0)
            {
                v0p = V1;
            }
            else
            {
                (v0p = V0) += (v0v1 *= s01);
            }
        }
        else if (s02 > 1.0)
        {
            s12 = dot(v1v2, (P - V1)) / sqrnorm(v1v2);
            if (s12 >= 1.0)
            {
                v0p = V2;
            }
            else if (s12 <= 0.0)
            {
                v0p = V1;
            }
            else
            {
                (v0p = V1) += (v1v2 *= s12);
            }
        }
        else
        {
            (v0p = V0) += (v0v2 *= s02);
        }
    }

    // Calculate the distance to an edge or a corner vertex
    else if (b < 0.0)
    {
        s01 = dot(v0v1, v0p) / sqrnorm(v0v1);
        if (s01 < 0.0)
        {
            s02 = dot(v0v2, v0p) / sqrnorm(v0v2);
            if (s02 <= 0.0)
            {
                v0p = V0;
            }
            else if (s02 >= 1.0)
            {
                v0p = V2;
            }
            else
            {
                (v0p = V0) += (v0v2 *= s02);
            }
        }
        else if (s01 > 1.0)
        {
            s12 = dot(v1v2, (P - V1)) / sqrnorm(v1v2);
            if (s12 >= 1.0)
            {
                v0p = V2;
            }
            else if (s12 <= 0.0)
            {
                v0p = V1;
            }
            else
            {
                (v0p = V1) += (v1v2 *= s12);
            }
        }
        else
        {
            (v0p = V0) += (v0v1 *= s01);
        }
    }

    // Calculate the distance to an edge or a corner vertex
    else if (a + b > 1.0)
    {
        s12 = dot(v1v2, (P - V1)) / sqrnorm(v1v2);
        if (s12 >= 1.0)
        {
            s02 = dot(v0v2, v0p) / sqrnorm(v0v2);
            if (s02 <= 0.0)
            {
                v0p = V0;
            }
            else if (s02 >= 1.0)
            {
                v0p = V2;
            }
            else
            {
                (v0p = V0) += (v0v2 *= s02);
            }
        }
        else if (s12 <= 0.0)
        {
            s01 = dot(v0v1, v0p) / sqrnorm(v0v1);
            if (s01 <= 0.0)
            {
                v0p = V0;
            }
            else if (s01 >= 1.0)
            {
                v0p = V1;
            }
            else
            {
                (v0p = V0) += (v0v1 *= s01);
            }
        }
        else
        {
            (v0p = V1) += (v1v2 *= s12);
        }
    }

    // Calculate the distance to an interior point of the triangle
    else
    {
        n *= (dot(n, v0p) * invD);
        (v0p = P) -= n;
    }

    NearestPoint = v0p;
    v0p -= P;
    return norm(v0p);
}

//=============================================================================
} // namespace pmp
//=============================================================================
