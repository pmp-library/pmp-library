//=============================================================================
// Copyright (C) 2011-2016 by Graphics & Geometry Group, Bielefeld University
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
#pragma once
//=============================================================================

#include <surface_mesh/Vector.h>

//=============================================================================

namespace surface_mesh {

//=============================================================================

template <typename Scalar>
const Vector<Scalar, 3> barycentricCoordinates(const Vector<Scalar, 3>& p,
                                               const Vector<Scalar, 3>& u,
                                               const Vector<Scalar, 3>& v,
                                               const Vector<Scalar, 3>& w)
{
    Vector<Scalar, 3> result(1.0 / 3.0); // default: barycenter

    Vector<Scalar, 3> vu = v - u, wu = w - u, pu = p - u;

    // find largest absolute coodinate of normal
    Scalar nx = vu[1] * wu[2] - vu[2] * wu[1],
           ny = vu[2] * wu[0] - vu[0] * wu[2],
           nz = vu[0] * wu[1] - vu[1] * wu[0], ax = fabs(nx), ay = fabs(ny),
           az = fabs(nz);

    unsigned char maxCoord;

    if (ax > ay)
    {
        if (ax > az)
        {
            maxCoord = 0;
        }
        else
        {
            maxCoord = 2;
        }
    }
    else
    {
        if (ay > az)
        {
            maxCoord = 1;
        }
        else
        {
            maxCoord = 2;
        }
    }

    // solve 2D problem
    switch (maxCoord)
    {
        case 0:
        {
            if (1.0 + ax != 1.0)
            {
                result[1] = 1.0 + (pu[1] * wu[2] - pu[2] * wu[1]) / nx - 1.0;
                result[2] = 1.0 + (vu[1] * pu[2] - vu[2] * pu[1]) / nx - 1.0;
                result[0] = 1.0 - result[1] - result[2];
            }
            break;
        }

        case 1:
        {
            if (1.0 + ay != 1.0)
            {
                result[1] = 1.0 + (pu[2] * wu[0] - pu[0] * wu[2]) / ny - 1.0;
                result[2] = 1.0 + (vu[2] * pu[0] - vu[0] * pu[2]) / ny - 1.0;
                result[0] = 1.0 - result[1] - result[2];
            }
            break;
        }

        case 2:
        {
            if (1.0 + az != 1.0)
            {
                result[1] = 1.0 + (pu[0] * wu[1] - pu[1] * wu[0]) / nz - 1.0;
                result[2] = 1.0 + (vu[0] * pu[1] - vu[1] * pu[0]) / nz - 1.0;
                result[0] = 1.0 - result[1] - result[2];
            }
            break;
        }
    }

    return result;
}

//=============================================================================
} // namespace surface_mesh
//=============================================================================
