// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include "pmp/mat_vec.h"

namespace pmp {

template <typename Scalar>
const Vector<Scalar, 3> barycentric_coordinates(const Vector<Scalar, 3>& p,
                                                const Vector<Scalar, 3>& u,
                                                const Vector<Scalar, 3>& v,
                                                const Vector<Scalar, 3>& w)
{
    Vector<Scalar, 3> result(Scalar(1.0 / 3.0)); // default: barycenter

    Vector<Scalar, 3> vu = v - u, wu = w - u, pu = p - u;

    // find largest absolute coordinate of normal
    Scalar nx = vu[1] * wu[2] - vu[2] * wu[1],
           ny = vu[2] * wu[0] - vu[0] * wu[2],
           nz = vu[0] * wu[1] - vu[1] * wu[0], ax = fabs(nx), ay = fabs(ny),
           az = fabs(nz);

    unsigned char max_coord;

    if (ax > ay)
    {
        if (ax > az)
        {
            max_coord = 0;
        }
        else
        {
            max_coord = 2;
        }
    }
    else
    {
        if (ay > az)
        {
            max_coord = 1;
        }
        else
        {
            max_coord = 2;
        }
    }

    // solve 2D problem
    switch (max_coord)
    {
        case 0:
        {
            if (1.0 + ax != 1.0)
            {
                result[1] = static_cast<Scalar>(
                    1.0 + (pu[1] * wu[2] - pu[2] * wu[1]) / nx - 1.0);
                result[2] = static_cast<Scalar>(
                    1.0 + (vu[1] * pu[2] - vu[2] * pu[1]) / nx - 1.0);
                result[0] = static_cast<Scalar>(1.0 - result[1] - result[2]);
            }
            break;
        }

        case 1:
        {
            if (1.0 + ay != 1.0)
            {
                result[1] = static_cast<Scalar>(
                    1.0 + (pu[2] * wu[0] - pu[0] * wu[2]) / ny - 1.0);
                result[2] = static_cast<Scalar>(
                    1.0 + (vu[2] * pu[0] - vu[0] * pu[2]) / ny - 1.0);
                result[0] = static_cast<Scalar>(1.0 - result[1] - result[2]);
            }
            break;
        }

        case 2:
        {
            if (1.0 + az != 1.0)
            {
                result[1] = static_cast<Scalar>(
                    1.0 + (pu[0] * wu[1] - pu[1] * wu[0]) / nz - 1.0);
                result[2] = static_cast<Scalar>(
                    1.0 + (vu[0] * pu[1] - vu[1] * pu[0]) / nz - 1.0);
                result[0] = static_cast<Scalar>(1.0 - result[1] - result[2]);
            }
            break;
        }
    }

    return result;
}

} // namespace pmp
