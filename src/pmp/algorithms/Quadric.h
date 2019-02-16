//=============================================================================
// Copyright (C) 2011-2019 The pmp-library developers
//
// This file is part of the Polygon Mesh Processing Library.
// Distributed under the terms of the MIT license, see LICENSE.txt for details.
//
// SPDX-License-Identifier: MIT
//=============================================================================
#pragma once
//=============================================================================

#include <pmp/Types.h>

//=============================================================================

namespace pmp {

//=============================================================================

//! \addtogroup algorithms algorithms
//!@{

//! This class stores a quadric as a symmetrix 4x4 matrix. Used by the error
//! quadric mesh decimation algorithms.
class Quadric
{
public: // clang-format off

    //! construct quadric from upper triangle of symmetrix 4x4 matrix
    Quadric(double a, double b, double c, double d,
            double e, double f, double g,
            double h, double i,
            double j)
        : a_(a), b_(b), c_(c), d_(d),
          e_(e), f_(f), g_(g),
          h_(h), i_(i),
          j_(j)
    {}

    //! constructor quadric from given plane equation: ax+by+cz+d=0
    Quadric(double a=0.0, double b=0.0, double c=0.0, double d=0.0)
        :  a_(a*a), b_(a*b), c_(a*c),  d_(a*d),
           e_(b*b), f_(b*c), g_(b*d),
           h_(c*c), i_(c*d),
           j_(d*d)
    {}

    //! construct from point and normal specifying a plane
    Quadric(const Normal& n, const Point& p)
    {
        *this = Quadric(n[0], n[1], n[2], -dot(n,p));
    }

    //! set all matrix entries to zero
    void clear() { a_ = b_ = c_ = d_ = e_ = f_ = g_ = h_ = i_ = j_ = 0.0; }

    //! add given quadric to this quadric
    Quadric& operator+=(const Quadric& q)
    {
        a_ += q.a_; b_ += q.b_; c_ += q.c_; d_ += q.d_;
        e_ += q.e_; f_ += q.f_; g_ += q.g_;
        h_ += q.h_; i_ += q.i_;
        j_ += q.j_;
        return *this;
    }

    //! multiply quadric by a scalar
    Quadric& operator*=(double s)
    {
        a_ *= s; b_ *= s; c_ *= s;  d_ *= s;
        e_ *= s; f_ *= s; g_ *= s;
        h_ *= s; i_ *= s;
        j_ *= s;
        return *this;
    }

    //! evaluate quadric Q at position p by computing (p^T * Q * p)
    double operator()(const Point& p) const
    {
        const double x(p[0]), y(p[1]), z(p[2]);
        return a_*x*x + 2.0*b_*x*y + 2.0*c_*x*z + 2.0*d_*x
            +  e_*y*y + 2.0*f_*y*z + 2.0*g_*y
            +  h_*z*z + 2.0*i_*z
            +  j_;
    }

private:

    double a_, b_, c_, d_,
        e_, f_, g_,
        h_, i_,
        j_;
}; // clang-format on

//=============================================================================
//!@}
//=============================================================================
} // namespace pmp
//=============================================================================
