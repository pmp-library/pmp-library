//=============================================================================
// Copyright (C) 2001-2005 by Computer Graphics Group, RWTH Aachen
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
#pragma once
//=============================================================================

#include <pmp/Vector.h>
#include <cmath>
#include <iostream>

//=============================================================================

namespace pmp {

//=============================================================================


//=============================================================================

template <class Scalar>
class Mat4
{
public:

    /// constructor
    Mat4() {}

    /// construct from 4 column vectors
    Mat4(Vector<Scalar,4> c0, Vector<Scalar,4> c1, Vector<Scalar,4> c2, Vector<Scalar,4> c3)
    {
        (*this)(0,0) = c0[0]; (*this)(0,1) = c1[0]; (*this)(0,2) = c2[0]; (*this)(0,3) = c3[0];
        (*this)(1,0) = c0[1]; (*this)(1,1) = c1[1]; (*this)(1,2) = c2[1]; (*this)(1,3) = c3[1];
        (*this)(2,0) = c0[2]; (*this)(2,1) = c1[2]; (*this)(2,2) = c2[2]; (*this)(2,3) = c3[2];
        (*this)(3,0) = c0[3]; (*this)(3,1) = c1[3]; (*this)(3,2) = c2[3]; (*this)(3,3) = c3[3];
    }

    /// construct from 16 (row-wise) entries
    Mat4(Scalar m00, Scalar m01, Scalar m02, Scalar m03,
         Scalar m10, Scalar m11, Scalar m12, Scalar m13,
         Scalar m20, Scalar m21, Scalar m22, Scalar m23,
         Scalar m30, Scalar m31, Scalar m32, Scalar m33)
    {
        (*this)(0,0) = m00; (*this)(0,1) = m01; (*this)(0,2) = m02; (*this)(0,3) = m03;
        (*this)(1,0) = m10; (*this)(1,1) = m11; (*this)(1,2) = m12; (*this)(1,3) = m13;
        (*this)(2,0) = m20; (*this)(2,1) = m21; (*this)(2,2) = m22; (*this)(2,3) = m23;
        (*this)(3,0) = m30; (*this)(3,1) = m31; (*this)(3,2) = m32; (*this)(3,3) = m33;
    }

    /// construct from scalar. sets all matrix entries to s.
    Mat4(Scalar s)
    {
        int i;
        for (i=0; i<16; ++i)
        {
            data_[i] = s;
        }
    }

    /// cast to matrix of other scalar type
    template <typename T> explicit operator Mat4<T>()
    {
        Mat4<T> m;
        for (int i=0; i<4; ++i)
            for (int j=0; j<4; ++j)
                m(i,j) = static_cast<T>((*this)(i,j));
        return m;
    }

    /// destructor
    ~Mat4() {}

    /// access entry at row i and column j
    Scalar& operator()(unsigned int i, unsigned int j)
    {
        return data_[(j<<2) + i];
    }

    /// const-access entry at row i and column j
    const Scalar& operator()(unsigned int i, unsigned int j) const
    {
        return data_[(j<<2) + i];
    }

    /// this = s / this
    Mat4& operator/=(const Scalar s)
    {
        int i,j; Scalar is(1.0/s);
        for (i=0; i<4; ++i) for (j=0; j<4; ++j)  (*this)(i,j) *= is;
        return *this;
    }

    /// this = s * this
    Mat4& operator*=(const Scalar s)
    {
        int i,j;
        for (i=0; i<4; ++i) for (j=0; j<4; ++j)  (*this)(i,j) *= s;
        return *this;
    }


    // matrix += matrix
    Mat4& operator+=(const Mat4& _M)
    {
        for (unsigned int i=0; i<4; ++i)
            for (unsigned int j=0; j<4; ++j)
                (*this)(i,j) += _M(i,j);
        return *this;
    }

    /// matrix + matrix
    const Mat4 operator+(const Mat4& _M) const
    {
        return Mat4(*this) += _M;
    }


    /// matrix -= matrix
    Mat4& operator-=(const Mat4& _M)
    {
        for (unsigned int i=0; i<4; ++i)
            for (unsigned int j=0; j<4; ++j)
                (*this)(i,j) -= _M(i,j);
        return *this;
    }

    /// matrix - matrix
    const Mat4 operator-(const Mat4& _M) const
    {
        return Mat4(*this) -= _M;
    }


    /// const-access as scalar array
    const Scalar* data() const { return data_; }

    /// access as scalar array
    Scalar* data()  { return data_; }

    /// return zero matrix
    static Mat4<Scalar> zero();

    /// return identity matrix
    static Mat4<Scalar> identity();

    static Mat4<Scalar> viewport(Scalar l, Scalar r, Scalar b, Scalar t);
    static Mat4<Scalar> inverse_viewport(Scalar l, Scalar r, Scalar b, Scalar t);

    static Mat4<Scalar> frustum(Scalar l, Scalar r, Scalar b, Scalar t, Scalar n, Scalar f);
    static Mat4<Scalar> inverse_frustum(Scalar l, Scalar r, Scalar b, Scalar t, Scalar n, Scalar f);

    static Mat4<Scalar> perspective(Scalar fovy, Scalar aspect, Scalar near, Scalar far);
    static Mat4<Scalar> inverse_perspective(Scalar fovy, Scalar aspect, Scalar near, Scalar far);

    static Mat4<Scalar> ortho(Scalar left, Scalar right, Scalar bottom, Scalar top, Scalar zNear, Scalar zFar);

    static Mat4<Scalar> look_at(const Vector<Scalar ,3> &eye, const Vector<Scalar ,3> &center, const Vector<Scalar ,3> &up);

    static Mat4<Scalar> translate(const Vector<Scalar ,3> &t);

    static Mat4<Scalar> rotate(const Vector<Scalar ,3> &axis, Scalar angle);
    static Mat4<Scalar> rotate_x(Scalar angle);
    static Mat4<Scalar> rotate_y(Scalar angle);
    static Mat4<Scalar> rotate_z(Scalar angle);


private:

    Scalar data_[16];
};





//-----------------------------------------------------------------------------


template <typename Scalar>
Mat4<Scalar>
Mat4<Scalar>::identity()
{
    Mat4<Scalar> m;

    for (int j=0; j<4; ++j)
        for (int i=0; i<4; ++i)
            m(i,j) = 0.0;

    m(0,0) = m(1,1) = m(2,2) = m(3,3) = 1.0;

    return m;
}



//-----------------------------------------------------------------------------


template <typename Scalar>
Mat4<Scalar>
Mat4<Scalar>::zero()
{
    Mat4<Scalar> m;

    for (int j=0; j<4; ++j)
        for (int i=0; i<4; ++i)
            m(i,j) = 0.0;

    return m;
}

//-----------------------------------------------------------------------------


template <typename Scalar>
Mat4<Scalar>
Mat4<Scalar>::viewport(Scalar l, Scalar b, Scalar w, Scalar h)
{
    Mat4<Scalar> m(Scalar(0));

    m(0,0) = 0.5*w;
    m(0,3) = 0.5*w + l;
    m(1,1) = 0.5*h;
    m(1,3) = 0.5*h + b;
    m(2,2) = 0.5;
    m(2,3) = 0.5;
    m(3,3) = 1.0f;

    return m;
}


template <typename Scalar>
Mat4<Scalar>
Mat4<Scalar>::inverse_viewport(Scalar l, Scalar b, Scalar w, Scalar h)
{
    Mat4<Scalar> m(Scalar(0));

    m(0,0) = 2.0/w;
    m(0,3) = -1.0 - (l+l)/w;
    m(1,1) = 2.0/h;
    m(1,3) = -1.0 - (b+b)/h;
    m(2,2) = 2.0;
    m(2,3) = -1.0;
    m(3,3) = 1.0f;

    return m;
}


//-----------------------------------------------------------------------------


template <typename Scalar>
Mat4<Scalar>
Mat4<Scalar>::frustum(Scalar l, Scalar r, Scalar b, Scalar t, Scalar n, Scalar f)
{
    Mat4<Scalar> m(Scalar(0));

    m(0,0) = (n+n) / (r-l);
    m(0,2) = (r+l) / (r-l);
    m(1,1) = (n+n) / (t-b);
    m(1,2) = (t+b) / (t-b);
    m(2,2) = -(f+n) / (f-n);
    m(2,3) = -f * (n+n) / (f-n);
    m(3,2) = -1.0f;

    return m;
}


template <typename Scalar>
Mat4<Scalar>
Mat4<Scalar>::inverse_frustum(Scalar l, Scalar r, Scalar b, Scalar t, Scalar n, Scalar f)
{
    Mat4<Scalar> m(Scalar(0));

    const Scalar nn = n+n;

    m(0,0) = (r-l) / nn;
    m(0,3) = (r+l) / nn;
    m(1,1) = (t-b) / nn;
    m(1,3) = (t+b) / nn;
    m(2,3) = -1.0;
    m(3,2) = (n-f) / (nn*f);
    m(3,3) = (n+f) / (nn*f);

    return m;
}


//-----------------------------------------------------------------------------


template <typename Scalar>
Mat4<Scalar>
Mat4<Scalar>::perspective(Scalar fovy, Scalar aspect, Scalar near, Scalar far)
{
    Scalar t = Scalar(near) * tan( fovy * M_PI / 360.0 );
    Scalar b = -t;
    Scalar l = b * aspect;
    Scalar r = t * aspect;

    return Mat4<Scalar>::frustum(l, r, b, t, Scalar(near), Scalar(far));
}


template <typename Scalar>
Mat4<Scalar>
Mat4<Scalar>::inverse_perspective(Scalar fovy, Scalar aspect, Scalar near, Scalar far)
{
    Scalar t = near * tan( fovy * M_PI / 360.0 );
    Scalar b = -t;
    Scalar l = b * aspect;
    Scalar r = t * aspect;

    return Mat4<Scalar>::inverse_frustum(l, r, b, t, near, far);
}


//-----------------------------------------------------------------------------

template <typename Scalar>
Mat4<Scalar>
Mat4<Scalar>::ortho(Scalar left, Scalar right, Scalar bottom, Scalar top, Scalar zNear, Scalar zFar)
{
    Mat4<Scalar> m(0.0);

    m(0,0) = Scalar(2) / (right - left);
    m(1,1) = Scalar(2) / (top - bottom);
    m(2,2) = - Scalar(2) / (zFar - zNear);
    m(0,3) = - (right + left) / (right - left);
    m(1,3) = - (top + bottom) / (top - bottom);
    m(2,3) = - (zFar + zNear) / (zFar - zNear);
    m(3,3) = Scalar(1);

    return m;
}

//-----------------------------------------------------------------------------

template <typename Scalar>
Mat4<Scalar>
Mat4<Scalar>::look_at(const Vector<Scalar ,3>& eye, const Vector<Scalar ,3>& center, const Vector<Scalar ,3>& up)
{

    Vector<Scalar,3> z = normalize(eye-center);
    Vector<Scalar,3> x = normalize(cross(up, z));
    Vector<Scalar,3> y = normalize(cross(z, x));

    Mat4<Scalar> m;
    m(0,0)=x[0]; m(0,1)=x[1]; m(0,2)=x[2]; m(0,3)=-dot(x,eye);
    m(1,0)=y[0]; m(1,1)=y[1]; m(1,2)=y[2]; m(1,3)=-dot(y,eye);
    m(2,0)=z[0]; m(2,1)=z[1]; m(2,2)=z[2]; m(2,3)=-dot(z,eye);
    m(3,0)=0.0;  m(3,1)=0.0;  m(3,2)=0.0;  m(3,3)=1.0;

    return m;
}

//-----------------------------------------------------------------------------

template <typename Scalar>
Mat4<Scalar>
Mat4<Scalar>::translate(const Vector<Scalar ,3>& t)
{
    Mat4<Scalar> m(Scalar(0));
    m(0,0) = m(1,1) = m(2,2) = m(3,3) = 1.0f;
    m(0,3) = t[0];
    m(1,3) = t[1];
    m(2,3) = t[2];

    return m;
}

//-----------------------------------------------------------------------------

template <typename Scalar>
Mat4<Scalar>
Mat4<Scalar>::rotate_x(Scalar angle)
{
  Scalar ca = cos(angle * (M_PI/180.0));
  Scalar sa = sin(angle * (M_PI/180.0));

  Mat4<Scalar> m(0.0);
  m(0,0) = 1.0;
  m(1,1) = ca;
  m(1,2) = -sa;
  m(2,2) = ca;
  m(2,1) = sa;
  m(3,3) = 1.0;

  return m;
}

//-----------------------------------------------------------------------------

template <typename Scalar>
Mat4<Scalar>
Mat4<Scalar>::rotate_y(Scalar angle)
{
  Scalar ca = cos(angle * (M_PI/180.0));
  Scalar sa = sin(angle * (M_PI/180.0));

  Mat4<Scalar> m(0.0);
  m(0,0) = ca;
  m(0,2) = sa;
  m(1,1) = 1.0;
  m(2,0) = -sa;
  m(2,2) = ca;
  m(3,3) = 1.0;

  return m;
}

//-----------------------------------------------------------------------------

template <typename Scalar>
Mat4<Scalar>
Mat4<Scalar>::rotate_z(Scalar angle)
{
  Scalar ca = cos(angle * (M_PI/180.0));
  Scalar sa = sin(angle * (M_PI/180.0));

  Mat4<Scalar> m(0.0);
  m(0,0) = ca;
  m(0,1) = -sa;
  m(1,0) = sa;
  m(1,1) = ca;
  m(2,2) = 1.0;
  m(3,3) = 1.0;

  return m;
}

//-----------------------------------------------------------------------------

template <typename Scalar>
Mat4<Scalar>
Mat4<Scalar>::rotate(const Vector<Scalar ,3>& axis, Scalar angle)
{
    Mat4<Scalar> m(Scalar(0));
    Scalar a = angle * (M_PI/180.0f);
    Scalar c = cosf(a);
    Scalar s = sinf(a);
    Scalar one_m_c = Scalar(1) - c;
    Vector<Scalar, 3> ax = normalize(axis);

    m(0,0) = ax[0]*ax[0] * one_m_c + c;
    m(0,1) = ax[0]*ax[1] * one_m_c - ax[2] * s;
    m(0,2) = ax[0]*ax[2] * one_m_c + ax[1] * s;

    m(1,0) = ax[1]*ax[0] * one_m_c + ax[2] * s;
    m(1,1) = ax[1]*ax[1] * one_m_c + c;
    m(1,2) = ax[1]*ax[2] * one_m_c - ax[0] * s;

    m(2,0) = ax[2]*ax[0] * one_m_c - ax[1] * s;
    m(2,1) = ax[2]*ax[1] * one_m_c + ax[0] * s;
    m(2,2) = ax[2]*ax[2] * one_m_c + c;

    m(3,3) = 1.0f;

    return m;
}

//-----------------------------------------------------------------------------


/// output matrix to ostream os
template <typename Scalar>
std::ostream&
operator<<(std::ostream& os, const Mat4<Scalar>& m)
{
    os << "# 4x4 matrix" << std::endl;
    for(int i=0; i<4; i++)
    {
        for(int j=0; j<4; j++)
            os << m(i,j) << " ";
        os << "\n";
    }
    return os;
}


//-----------------------------------------------------------------------------


/// read the space-separated components of a vector from a stream */
template <typename Scalar>
std::istream&
operator>>(std::istream& is, Mat4<Scalar>& m)
{
    std::string comment;
    getline(is,comment);
    for(int i=0; i<4; i++)
        for(int j=0; j<4; j++)
            is >> m(i,j);
    return is;
}


//-----------------------------------------------------------------------------


template <typename Scalar>
Vector<Scalar,3>
projective_transform(const Mat4<Scalar>& m, const Vector<Scalar,3>& v)
{
    const Scalar x = m(0,0)*v[0] + m(0,1)*v[1] + m(0,2)*v[2] + m(0,3);
    const Scalar y = m(1,0)*v[0] + m(1,1)*v[1] + m(1,2)*v[2] + m(1,3);
    const Scalar z = m(2,0)*v[0] + m(2,1)*v[1] + m(2,2)*v[2] + m(2,3);
    const Scalar w = m(3,0)*v[0] + m(3,1)*v[1] + m(3,2)*v[2] + m(3,3);
    return Vector<Scalar,3>(x/w,y/w,z/w);
}


//-----------------------------------------------------------------------------


template <typename Scalar>
Vector<Scalar,3>
affine_transform(const Mat4<Scalar>& m, const Vector<Scalar,3>& v)
{
    const Scalar x = m(0,0)*v[0] + m(0,1)*v[1] + m(0,2)*v[2] + m(0,3);
    const Scalar y = m(1,0)*v[0] + m(1,1)*v[1] + m(1,2)*v[2] + m(1,3);
    const Scalar z = m(2,0)*v[0] + m(2,1)*v[1] + m(2,2)*v[2] + m(2,3);
    return Vector<Scalar,3>(x,y,z);
}


//-----------------------------------------------------------------------------


template <typename Scalar>
Vector<Scalar,3>
linear_transform(const Mat4<Scalar>& m, const Vector<Scalar,3>& v)
{
    const Scalar x = m(0,0)*v[0] + m(0,1)*v[1] + m(0,2)*v[2];
    const Scalar y = m(1,0)*v[0] + m(1,1)*v[1] + m(1,2)*v[2];
    const Scalar z = m(2,0)*v[0] + m(2,1)*v[1] + m(2,2)*v[2];
    return Vector<Scalar,3>(x,y,z);
}


//-----------------------------------------------------------------------------


template <typename Scalar>
Mat4<Scalar>
operator*(const Mat4<Scalar>& m0, const Mat4<Scalar>& m1)
{
    Mat4<Scalar> m;

    for (int i=0; i<4; ++i)
    {
        for (int j=0; j<4; ++j)
        {
            m(i,j) = Scalar(0);
            for (int k=0; k<4; ++k)
                m(i,j) += m0(i,k) * m1(k,j);
        }
    }

    return m;
}


//-----------------------------------------------------------------------------


template <typename Scalar>
Vector<Scalar,4>
operator*(const Mat4<Scalar>& m, const Vector<Scalar,4>& v)
{
    const Scalar x = m(0,0)*v[0] + m(0,1)*v[1] + m(0,2)*v[2] + m(0,3)*v[3];
    const Scalar y = m(1,0)*v[0] + m(1,1)*v[1] + m(1,2)*v[2] + m(1,3)*v[3];
    const Scalar z = m(2,0)*v[0] + m(2,1)*v[1] + m(2,2)*v[2] + m(2,3)*v[3];
    const Scalar w = m(3,0)*v[0] + m(3,1)*v[1] + m(3,2)*v[2] + m(3,3)*v[3];

    return Vector<Scalar,4> (x,y,z,w);
}


//-----------------------------------------------------------------------------


template <typename Scalar>
Mat4<Scalar>
inverse(const Mat4<Scalar>& m)
{

    Scalar Coef00 = m(2,2) * m(3,3) - m(2,3) * m(3,2);
    Scalar Coef02 = m(2,1) * m(3,3) - m(2,3) * m(3,1);
    Scalar Coef03 = m(2,1) * m(3,2) - m(2,2) * m(3,1);

    Scalar Coef04 = m(1,2) * m(3,3) - m(1,3) * m(3,2);
    Scalar Coef06 = m(1,1) * m(3,3) - m(1,3) * m(3,1);
    Scalar Coef07 = m(1,1) * m(3,2) - m(1,2) * m(3,1);

    Scalar Coef08 = m(1,2) * m(2,3) - m(1,3) * m(2,2);
    Scalar Coef10 = m(1,1) * m(2,3) - m(1,3) * m(2,1);
    Scalar Coef11 = m(1,1) * m(2,2) - m(1,2) * m(2,1);

    Scalar Coef12 = m(0,2) * m(3,3) - m(0,3) * m(3,2);
    Scalar Coef14 = m(0,1) * m(3,3) - m(0,3) * m(3,1);
    Scalar Coef15 = m(0,1) * m(3,2) - m(0,2) * m(3,1);

    Scalar Coef16 = m(0,2) * m(2,3) - m(0,3) * m(2,2);
    Scalar Coef18 = m(0,1) * m(2,3) - m(0,3) * m(2,1);
    Scalar Coef19 = m(0,1) * m(2,2) - m(0,2) * m(2,1);

    Scalar Coef20 = m(0,2) * m(1,3) - m(0,3) * m(1,2);
    Scalar Coef22 = m(0,1) * m(1,3) - m(0,3) * m(1,1);
    Scalar Coef23 = m(0,1) * m(1,2) - m(0,2) * m(1,1);

    Vector<Scalar,4> const SignA(+1, -1, +1, -1);
    Vector<Scalar,4> const SignB(-1, +1, -1, +1);

    Vector<Scalar,4> Fac0(Coef00, Coef00, Coef02, Coef03);
    Vector<Scalar,4> Fac1(Coef04, Coef04, Coef06, Coef07);
    Vector<Scalar,4> Fac2(Coef08, Coef08, Coef10, Coef11);
    Vector<Scalar,4> Fac3(Coef12, Coef12, Coef14, Coef15);
    Vector<Scalar,4> Fac4(Coef16, Coef16, Coef18, Coef19);
    Vector<Scalar,4> Fac5(Coef20, Coef20, Coef22, Coef23);

    Vector<Scalar,4> Vec0(m(0,1), m(0,0), m(0,0), m(0,0));
    Vector<Scalar,4> Vec1(m(1,1), m(1,0), m(1,0), m(1,0));
    Vector<Scalar,4> Vec2(m(2,1), m(2,0), m(2,0), m(2,0));
    Vector<Scalar,4> Vec3(m(3,1), m(3,0), m(3,0), m(3,0));

    Vector<Scalar,4> Inv0 = SignA * (Vec1 * Fac0 - Vec2 * Fac1 + Vec3 * Fac2);
    Vector<Scalar,4> Inv1 = SignB * (Vec0 * Fac0 - Vec2 * Fac3 + Vec3 * Fac4);
    Vector<Scalar,4> Inv2 = SignA * (Vec0 * Fac1 - Vec1 * Fac3 + Vec3 * Fac5);
    Vector<Scalar,4> Inv3 = SignB * (Vec0 * Fac2 - Vec1 * Fac4 + Vec2 * Fac5);

    Mat4<Scalar> Inverse(Inv0, Inv1, Inv2, Inv3);

    Vector<Scalar,4> Row0(Inverse(0,0), Inverse(1,0), Inverse(2,0), Inverse(3,0));
    Vector<Scalar,4> Col0(m(0,0), m(0,1), m(0,2), m(0,3));

    Scalar Determinant = dot(Col0, Row0);

    Inverse /= Determinant;

    return Inverse;
}


//-----------------------------------------------------------------------------


template <typename Scalar>
Mat4<Scalar>
transpose(const Mat4<Scalar>& m)
{
    Mat4<Scalar> result;

    for (int j=0; j<4; ++j)
        for (int i=0; i<4; ++i)
            result(i,j) = m(j,i);

    return result;
}


//=============================================================================


template <class Scalar>
class Mat3
{
public:

    /// constructor
    Mat3() {}

    Mat3(Scalar s)
    {
        int i,j;
        for (i=0; i<3; ++i) {
            for (j=0; j<3; ++j) {
                data_[j*3+i]= s;
            }
        }
    }

    Mat3(bool identity) :
        Mat3(Scalar(0))
    {
        if (identity) {
            (*this)(0,0) = (*this)(1,1) = (*this)(2,2) = (*this)(3,3) = 1.0;
        }
    }

    Mat3(const Mat4<Scalar>& m)
    {
        int i,j;
        for (i=0; i<3; ++i)
            for (j=0; j<3; ++j)
                (*this)(i,j) = m(i,j);
    }

    /// destructor
    ~Mat3() {}

    /// access entry at row i and column j
    Scalar& operator()(unsigned int i, unsigned int j)
    {
        return data_[j*3+i];
    }

    /// const-access entry at row i and column j
    const Scalar& operator()(unsigned int i, unsigned int j) const
    {
        return data_[j*3+i];
    }

    /// return zero matrix
    static Mat3<Scalar> zero();

    /// return identity matrix
    static Mat3<Scalar> identity();

    /// this = this * m
    Mat3& operator*=(const Mat3& m)
    {
        return (*this = *this * m);
    };

    /// this = this + m
    Mat3& operator+=(const Mat3& m)
    {
        int i,j;
        for (i=0; i<3; ++i) for (j=0; j<3; ++j)  data_[j*3+i] += m.data_[j*3+i];
        return *this;
    };

    /// this = this - m
    Mat3& operator-=(const Mat3& m)
    {
        int i,j;
        for (i=0; i<3; ++i) for (j=0; j<3; ++j)  data_[j*3+i] -= m.data_[j*3+i];
        return *this;
    };

    /// this = s * this
    Mat3& operator*=(const Scalar s)
    {
        int i,j;
        for (i=0; i<3; ++i) for (j=0; j<3; ++j)  data_[j*3+i] *= s;
        return *this;
    };

    /// this = s / this
    Mat3& operator/=(const Scalar s)
    {
        int i,j; Scalar is(1.0/s);
        for (i=0; i<3; ++i) for (j=0; j<3; ++j)  data_[j*3+i] *= is;
        return *this;
    };

    /// Frobenius norm
    double norm() const
    {
        double s(0.0);
        for (unsigned int i=0; i<3; ++i)
            for (unsigned int j=0; j<3; ++j)
                s += (*this)(i,j) * (*this)(i,j);
        return sqrt(s);
    }

    /// trace
    double trace() const
    {
        return (*this)(0,0) + (*this)(1,1) + (*this)(2,2);
    }



    /// const access to array
    const Scalar* data() const { return data_; }

    /// acces to array
    Scalar* data() { return data_; }

private:

    Scalar data_[9];
};



//-----------------------------------------------------------------------------


template <typename Scalar>
Mat3<Scalar>
Mat3<Scalar>::identity()
{
    Mat3<Scalar> m;

    for (int j=0; j<3; ++j)
        for (int i=0; i<3; ++i)
            m(i,j) = 0.0;

    m(0,0) = m(1,1) = m(2,2) = 1.0;

    return m;
}


//-----------------------------------------------------------------------------


template <typename Scalar>
Mat3<Scalar>
Mat3<Scalar>::zero()
{
    Mat3<Scalar> m;

    for (int j=0; j<3; ++j)
        for (int i=0; i<3; ++i)
            m(i,j) = 0.0;

    return m;
}


//-----------------------------------------------------------------------------


template <typename Scalar>
Mat3<Scalar>
inverse(const Mat3<Scalar> &m)
{
    Scalar det = (- m(0,0)*m(1,1)*m(2,2)
                 + m(0,0)*m(1,2)*m(2,1)
                 + m(1,0)*m(0,1)*m(2,2)
                 - m(1,0)*m(0,2)*m(2,1)
                 - m(2,0)*m(0,1)*m(1,2)
                 + m(2,0)*m(0,2)*m(1,1));

    Mat3<Scalar> inv;
    inv(0,0) = (m(1,2)*m(2,1) - m(1,1)*m(2,2)) / det;
    inv(0,1) = (m(0,1)*m(2,2) - m(0,2)*m(2,1)) / det;
    inv(0,2) = (m(0,2)*m(1,1) - m(0,1)*m(1,2)) / det;
    inv(1,0) = (m(1,0)*m(2,2) - m(1,2)*m(2,0)) / det;
    inv(1,1) = (m(0,2)*m(2,0) - m(0,0)*m(2,2)) / det;
    inv(1,2) = (m(0,0)*m(1,2) - m(0,2)*m(1,0)) / det;
    inv(2,0) = (m(1,1)*m(2,0) - m(1,0)*m(2,1)) / det;
    inv(2,1) = (m(0,0)*m(2,1) - m(0,1)*m(2,0)) / det;
    inv(2,2) = (m(0,1)*m(1,0) - m(0,0)*m(1,1)) / det;

    return inv;
}


//-----------------------------------------------------------------------------



template <typename Scalar>
Vector<Scalar,3>
operator*(const Mat3<Scalar>& m, const Vector<Scalar,3>& v)
{
    const Scalar x = m(0,0)*v[0] + m(0,1)*v[1] + m(0,2)*v[2];
    const Scalar y = m(1,0)*v[0] + m(1,1)*v[1] + m(1,2)*v[2];
    const Scalar z = m(2,0)*v[0] + m(2,1)*v[1] + m(2,2)*v[2];

    return Vector<Scalar,3> (x,y,z);
}


//-----------------------------------------------------------------------------


template <typename Scalar>
const Mat3<Scalar>
outer_product(const Vector<Scalar,3>& a, const Vector<Scalar,3>& b)
{
    Mat3<Scalar> m;

    for (int j=0; j<3; ++j)
        for (int i=0; i<3; ++i)
            m(i,j) = a[i]*b[j];

    return m;
}


//-----------------------------------------------------------------------------


template <typename Scalar>
Mat3<Scalar>
transpose(const Mat3<Scalar>& m)
{
    Mat3<Scalar> result;

    for (int j=0; j<3; ++j)
        for (int i=0; i<3; ++i)
            result(i,j) = m(j,i);

    return result;
}


//-----------------------------------------------------------------------------


template <typename Scalar>
Mat3<Scalar>
operator*(const Mat3<Scalar>& m0, const Mat3<Scalar>& m1)
{
    Mat3<Scalar> m;

    for (int i=0; i<3; ++i)
    {
        for (int j=0; j<3; ++j)
        {
            m(i,j) = 0.0f;
            for (int k=0; k<3; ++k)
            {
                m(i,j) += m0(i,k) * m1(k,j);
            }
        }
    }

    return m;
}


//-----------------------------------------------------------------------------


/// output matrix to ostream os
template <typename Scalar>
std::ostream&
operator<<(std::ostream& os, const Mat3<Scalar>& m)
{
    for(int i=0; i<3; i++)
    {
        for(int j=0; j<3; j++)
            os << m(i,j) << " ";
        os << "\n";
    }
    return os;
}


//-----------------------------------------------------------------------------


/// read the space-separated components of a vector from a stream
template <typename Scalar>
std::istream&
operator>>(std::istream& is, Mat3<Scalar>& m)
{
    for(int i=0; i<3; i++)
        for(int j=0; j<3; j++)
            is >> m(i,j);
    return is;
}


//-----------------------------------------------------------------------------


template <typename Scalar>
bool
symmetricEigendecomposition(const Mat3<Scalar>& m,
                            Scalar& eval1,
                            Scalar& eval2,
                            Scalar& eval3,
                            Vector<Scalar,3>& evec1,
                            Vector<Scalar,3>& evec2,
                            Vector<Scalar,3>& evec3)
{
    unsigned int   i, j;
    Scalar         theta, t, c, s;
    Mat3<Scalar>   V = Mat3<Scalar>::identity();
    Mat3<Scalar>   R;
    Mat3<Scalar>   A=m;
    const Scalar   eps = 1e-10;//0.000001;


    int iterations = 100;
    while (iterations--)
    {
        // find largest off-diagonal elem
        if (fabs(A(0,1)) < fabs(A(0,2)))
        {
            if (fabs(A(0,2)) < fabs(A(1,2)))
            {
                i = 1, j = 2;
            }
            else
            {
                i = 0, j = 2;
            }
        }
        else
        {
            if (fabs(A(0,1)) < fabs(A(1,2)))
            {
                i = 1, j = 2;
            }
            else
            {
                i = 0, j = 1;
            }
        }


        // converged?
        if (fabs(A(i,j)) < eps) break;


        // compute Jacobi-Rotation
        theta = 0.5 * (A(j,j) - A(i,i)) / A(i,j);
        t = 1.0 / ( fabs( theta ) + sqrt( 1.0 + theta*theta ) );
        if (theta < 0.0) t = -t;

        c = 1.0 / sqrt(1.0 + t*t);
        s = t*c;

        R = Mat3<Scalar>::identity();
        R(i,i) = R(j,j) = c;
        R(i,j) = s;
        R(j,i) = -s;

        A = transpose(R) * A * R;
        V *= R;
    }


    if (iterations > 0)
    {

        // sort and return
        int sorted[3];
        Scalar d[3]={A(0,0), A(1,1), A(2,2)};

        if (d[0] > d[1])
        {
            if (d[1] > d[2])
            {
                sorted[0] = 0, sorted[1] = 1, sorted[2] = 2;
            }
            else
            {
                if (d[0] > d[2])
                {
                    sorted[0] = 0, sorted[1] = 2, sorted[2] = 1;
                }
                else
                {
                    sorted[0] = 2, sorted[1] = 0, sorted[2] = 1;
                }
            }
        }
        else
        {
            if (d[0] > d[2])
            {
                sorted[0] = 1, sorted[1] = 0, sorted[2] = 2;
            }
            else
            {
                if (d[1] > d[2])
                {
                    sorted[0] = 1, sorted[1] = 2, sorted[2] = 0;
                }
                else
                {
                    sorted[0] = 2, sorted[1] = 1, sorted[2] = 0;
                }
            }
        }

        eval1 = d[sorted[0]];
        eval2 = d[sorted[1]];
        eval3 = d[sorted[2]];

        evec1 = Vector<Scalar,3>(V(0,sorted[0]), V(1,sorted[0]), V(2,sorted[0]));
        evec2 = Vector<Scalar,3>(V(0,sorted[1]), V(1,sorted[1]), V(2,sorted[1]));
        evec3 = normalize(cross(evec1, evec2));

        return true;
    }


    return false;
}


//=============================================================================

typedef Mat3<float>    mat3;
typedef Mat3<double>  dmat3;

typedef Mat4<float>    mat4;
typedef Mat4<double>  dmat4;

//=============================================================================
} // namespace
//=============================================================================
