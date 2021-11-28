// Copyright 2011-2021 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include <vector>
#include "pmp/Types.h"

namespace pmp {

enum Registration_method
{
    RIGID_REGISTRATION,
    CONFORMAL_REGISTRATION
};

mat4 registration(const std::vector<Point>& _src,
                  const std::vector<Point>& _dst,
                  Registration_method _mapping = RIGID_REGISTRATION,
                  const std::vector<Scalar>* _weights = nullptr)
{
    assert(_src.size() == _dst.size());
    const int n = _src.size();
    assert(n > 2);

    // compute (weighted) barycenters
    Point scog(0.0, 0.0, 0.0), dcog(0.0, 0.0, 0.0);
    {
        Scalar ww(0.0);
        for (int i = 0; i < n; ++i)
        {
            const double w = _weights ? (*_weights)[i] : 1.0;
            scog += w * _src[i];
            dcog += w * _dst[i];
            ww += w;
        }
        scog /= ww;
        dcog /= ww;
    }

    // build 4x4 matrix
    dmat4 M;
    {
        double xx(0.0), xy(0.0), xz(0.0), yx(0.0), yy(0.0), yz(0.0), zx(0.0),
            zy(0.0), zz(0.0);

        for (int i = 0; i < n; ++i)
        {
            const dvec3 sp(_src[i] - scog);
            const dvec3 dp(_dst[i] - dcog);
            const double w = _weights ? (*_weights)[i] : 1.0;

            xx += w * sp[0] * dp[0];
            xy += w * sp[0] * dp[1];
            xz += w * sp[0] * dp[2];
            yx += w * sp[1] * dp[0];
            yy += w * sp[1] * dp[1];
            yz += w * sp[1] * dp[2];
            zx += w * sp[2] * dp[0];
            zy += w * sp[2] * dp[1];
            zz += w * sp[2] * dp[2];
        }

        M(0, 0) = xx + yy + zz;
        M(1, 1) = xx - yy - zz;
        M(2, 2) = -xx + yy - zz;
        M(3, 3) = -xx - yy + zz;
        M(1, 0) = M(0, 1) = yz - zy;
        M(2, 0) = M(0, 2) = zx - xz;
        M(2, 1) = M(1, 2) = xy + yx;
        M(3, 0) = M(0, 3) = xy - yx;
        M(3, 1) = M(1, 3) = zx + xz;
        M(3, 2) = M(2, 3) = yz + zy;
    }

    // symmetric eigendecomposition
    dmat4 V = dmat4::identity();
    unsigned int iter(50);
    {
        int i, j, k;
        double theta, t, c, s, ss, g, h, tau, tM;

        while (--iter)
        {
            // find largest off-diagonal element
            i = 0;
            j = 1;
            ss = fabs(M(0, 1));
            if ((s = fabs(M(0, 2))) > ss)
            {
                ss = s;
                i = 0;
                j = 2;
            }
            if ((s = fabs(M(0, 3))) > ss)
            {
                ss = s;
                i = 0;
                j = 3;
            }
            if ((s = fabs(M(1, 2))) > ss)
            {
                ss = s;
                i = 1;
                j = 2;
            }
            if ((s = fabs(M(1, 3))) > ss)
            {
                ss = s;
                i = 1;
                j = 3;
            }
            if ((s = fabs(M(2, 3))) > ss)
            {
                ss = s;
                i = 2;
                j = 3;
            }

            // converged?
            if (ss < 1e-10)
                break;

            // compute Jacobi rotation
            theta = 0.5 * (M(j, j) - M(i, i)) / M(i, j);
            t = (theta < 0.0 ? -1.0 : 1.0) /
                (fabs(theta) + sqrt(1.0 + theta * theta));
            c = 1.0 / sqrt(1.0 + t * t);
            s = t * c;
            tau = s / (1.0 + c);
            tM = t * M(i, j);

#define rot(a, s, t, i, j, k, l)       \
    {                                  \
        g = a(i, j);                   \
        h = a(k, l);                   \
        a(i, j) = g - s * (h + g * t); \
        a(k, l) = h + s * (g - h * t); \
    }

            M(i, j) = 0.0;
            for (k = 0; k < i; ++k)
                rot(M, s, tau, k, i, k, j);
            for (k = i + 1; k < j; ++k)
                rot(M, s, tau, i, k, k, j);
            for (k = j + 1; k < 4; ++k)
                rot(M, s, tau, i, k, j, k);
            for (k = 0; k < 4; ++k)
                rot(V, s, tau, k, i, k, j);
            M(i, i) -= tM;
            M(j, j) += tM;

#undef rot
        }
    }

    // did it work?
    if (!iter)
    {
        std::cerr << "Registration: Jacobi did not converge\n";
        return Mat4<Scalar>::identity();
    }

    // eigenvector wrt largest eigenvalue -> quaternion
    dvec4 q;
    {
        int imax = 0;
        double ss = M(imax, imax);
        for (int i=1; i<4; ++i)
        {
            if (M(i,i) > ss)
            {
                ss = M(i,i);
                imax = i;
            }
        }
        q = dvec4(V(0, imax), V(1, imax), V(2, imax), V(3, imax));
        q.normalize();
    }

    // rotation part
    Scalar ww(q[0] * q[0]), xx(q[1] * q[1]), yy(q[2] * q[2]), zz(q[3] * q[3]),
        wx(q[0] * q[1]), wy(q[0] * q[2]), wz(q[0] * q[3]), xy(q[1] * q[2]),
        xz(q[1] * q[3]), yz(q[2] * q[3]);
    mat4 T;
    T(0, 0) = ww + xx - yy - zz;
    T(1, 0) = 2.0 * (xy + wz);
    T(2, 0) = 2.0 * (xz - wy);
    T(3, 0) = 0.0;
    T(0, 1) = 2.0 * (xy - wz);
    T(1, 1) = ww - xx + yy - zz;
    T(2, 1) = 2.0 * (yz + wx);
    T(3, 1) = 0.0;
    T(0, 2) = 2.0 * (xz + wy);
    T(1, 2) = 2.0 * (yz - wx);
    T(2, 2) = ww - xx - yy + zz;
    T(3, 2) = 0.0;

    // scaling
    if (_mapping == CONFORMAL_REGISTRATION)
    {
        Point sp, dp;
        Scalar nom(0), denom(0);

        for (int i = 0; i < n; ++i)
        {
            sp = _src[i];
            sp -= scog;
            dp = _dst[i];
            dp -= dcog;
            sp = linear_transform(T, sp);
            nom += dot(sp, dp);
            denom += dot(sp, sp);
        }

        const Scalar scaling = nom / denom;

        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                T(i, j) *= scaling;
            }
        }
    }

    // translation part
    T(0, 3) =
        dcog[0] - T(0, 0) * scog[0] - T(0, 1) * scog[1] - T(0, 2) * scog[2];
    T(1, 3) =
        dcog[1] - T(1, 0) * scog[0] - T(1, 1) * scog[1] - T(1, 2) * scog[2];
    T(2, 3) =
        dcog[2] - T(2, 0) * scog[0] - T(2, 1) * scog[1] - T(2, 2) * scog[2];
    T(3, 3) = 1.0;

    return T;
}

mat4 registration_l1(const std::vector<Point>& _src,
                     const std::vector<Point>& _dst)
{
    mat4 result;

    assert(_src.size() == _dst.size());
    const int n = _src.size();
    assert(n > 2);

    std::vector<Scalar> weights(n, 1.0);
    std::vector<Scalar> l2_errors(n);
    Scalar max_l2_error;
    Scalar l1_error;
    Scalar prev_l1_error = std::numeric_limits<Scalar>::max();

    for (int iter = 0; iter < 100; ++iter)
    {
        const mat4 trafo = registration(_src, _dst, RIGID_REGISTRATION, &weights);

        max_l2_error = 0.0;
        l1_error = 0.0;

        for (int i = 0; i < n; ++i)
        {
            Scalar dist = distance(affine_transform(trafo, _src[i]), _dst[i]);
            l1_error += dist;
            l2_errors[i] = dist * dist;
            max_l2_error = std::max(max_l2_error, l2_errors[i]);
        }

        // std::cerr << l1_error << std::endl;
        if (l1_error > prev_l1_error)
            break;
        result = trafo;

        for (int i = 0; i < n; ++i)
        {
            weights[i] = std::min( Scalar(10000), max_l2_error / l2_errors[i] );
        }
    }

    return result;
}

} // namespace pmp
