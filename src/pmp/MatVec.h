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

#include <cmath>
#include <iostream>
#include <assert.h>
#include <limits>

//=============================================================================

namespace pmp {

//=============================================================================

/// Base class for MxN matrix
template <typename Scalar, int M, int N>
class Matrix
{
public:
    //! the scalar type of the vector
    typedef Scalar value_type;

    //! returns number of rows of the matrix
    static constexpr int rows() { return M; }
    //! returns number of columns of the matrix
    static constexpr int cols() { return N; }
    //! returns the dimension of the vector (or size of the matrix, rows*cols)
    static constexpr int size() { return M * N; }

    /// empty default constructor
    Matrix() {}

    /// construct with all entries being a given scalar (matrix and vector)
    explicit Matrix(Scalar s)
    {
        for (int i   = 0; i < size(); ++i)
            data_[i] = s;
    }

    /// constructor for 2D vectors
    explicit Matrix(Scalar x, Scalar y)
    {
        static_assert(M == 2 && N == 1, "only for 2D vectors");
        data_[0] = x;
        data_[1] = y;
    }

    /// constructor for 3D vectors
    explicit Matrix(Scalar x, Scalar y, Scalar z)
    {
        static_assert(M == 3 && N == 1, "only for 3D vectors");
        data_[0] = x;
        data_[1] = y;
        data_[2] = z;
    }

    /// constructor for 4D vectors
    explicit Matrix(Scalar x, Scalar y, Scalar z, Scalar w)
    {
        static_assert(M == 4 && N == 1, "only for 4D vectors");
        data_[0] = x;
        data_[1] = y;
        data_[2] = z;
        data_[3] = w;
    }

    /// constructor for 4D vectors
    explicit Matrix(Matrix<Scalar, 3, 1> xyz, Scalar w)
    {
        static_assert(M == 4 && N == 1, "only for 4D vectors");
        data_[0] = xyz[0];
        data_[1] = xyz[1];
        data_[2] = xyz[2];
        data_[3] = w;
    }

    /// copy constructor from other scalar type
    /// is also invoked for type-casting
    template <typename OtherScalarType>
    explicit Matrix(const Matrix<OtherScalarType, M, N>& m)
    {
        for (int i   = 0; i < size(); ++i)
            data_[i] = static_cast<Scalar>(m[i]);
    }

    /// return identity matrix (only for square matrices, N==M)
    static Matrix<Scalar, M, N> identity();

    /// access entry at row i and column j
    Scalar& operator()(unsigned int i, unsigned int j)
    {
        assert(i < M && j < N);
        return data_[M * j + i];
    }

    /// const-access entry at row i and column j
    const Scalar& operator()(unsigned int i, unsigned int j) const
    {
        assert(i < M && j < N);
        return data_[M * j + i];
    }

    /// access i'th entry (use for vectors)
    Scalar& operator[](unsigned int i)
    {
        assert(i < M * N);
        return data_[i];
    }

    /// const-access i'th entry (use for vectors)
    Scalar operator[](unsigned int i) const
    {
        assert(i < M * N);
        return data_[i];
    }

    /// const-access as scalar array
    const Scalar* data() const { return data_; }

    /// access as scalar array
    Scalar* data() { return data_; }

    /// normalize matrix/vector by dividing through Frobenius/Euclidean norm
    void normalize() { *this /= norm(*this); }

    /// divide matrix by scalar
    Matrix<Scalar, M, N>& operator/=(const Scalar s)
    {
        for (int i = 0; i < size(); ++i)
            data_[i] /= s;
        return *this;
    }

    /// multply matrix by scalar
    Matrix<Scalar, M, N>& operator*=(const Scalar s)
    {
        for (int i = 0; i < size(); ++i)
            data_[i] *= s;
        return *this;
    }

    // add other matrix to this matrix
    Matrix<Scalar, M, N>& operator+=(const Matrix<Scalar, M, N>& m)
    {
        for (int i = 0; i < size(); ++i)
            data_[i] += m.data_[i];
        return *this;
    }

    /// subtract other matrix from this matrix
    Matrix<Scalar, M, N>& operator-=(const Matrix<Scalar, M, N>& m)
    {
        for (int i = 0; i < size(); ++i)
            data_[i] -= m.data_[i];
        return *this;
    }

    //! component-wise comparison
    bool operator==(const Matrix<Scalar, M, N>& other) const
    {
        for (int i = 0; i < size(); ++i)
            if (data_[i] != other.data_[i])
                return false;
        return true;
    }

    //! component-wise comparison
    bool operator!=(const Matrix<Scalar, M, N>& other) const
    {
        for (int i = 0; i < size(); ++i)
            if (data_[i] != other.data_[i])
                return true;
        return false;
    }

    //! return matrix with minimum of this and other in each component
    //Matrix<Scalar,M,N> minimize(const Matrix<Scalar,M,N>& other)
    //{
    //for (int i=0; i<size(); ++i)
    //if (other[i] < data_[i])
    //data_[i] = other[i];
    //return *this;
    //}

    //! return matrix with maximum of this and other in each component
    //Matrix<Scalar,M,N> maximize(const Matrix<Scalar,M,N>& other)
    //{
    //for (int i=0; i<size(); ++i)
    //if (other[i] > data_[i])
    //data_[i] = other[i];
    //return *this;
    //}

protected:
    Scalar data_[N * M];
};

//== TEMPLATE SPECIALIZATIONS =================================================

/// template specialization for Vector as Nx1 matrix
template <typename Scalar, int M>
using Vector = Matrix<Scalar, M, 1>;

/// template specialization for 4x4 matrices
template <typename Scalar>
using Mat4 = Matrix<Scalar, 4, 4>;

/// template specialization for 3x3 matrices
template <typename Scalar>
using Mat3 = Matrix<Scalar, 3, 3>;

/// template specialization for 2x2 matrices
template <typename Scalar>
using Mat2 = Matrix<Scalar, 2, 2>;

//== TYPEDEFS =================================================================

typedef Vector<float, 2>        vec2;
typedef Vector<double, 2>       dvec2;
typedef Vector<bool, 2>         bvec2;
typedef Vector<int, 2>          ivec2;
typedef Vector<unsigned int, 2> uvec2;

typedef Vector<float, 3>        vec3;
typedef Vector<double, 3>       dvec3;
typedef Vector<bool, 3>         bvec3;
typedef Vector<int, 3>          ivec3;
typedef Vector<unsigned int, 3> uvec3;

typedef Vector<float, 4>        vec4;
typedef Vector<double, 4>       dvec4;
typedef Vector<bool, 4>         bvec4;
typedef Vector<int, 4>          ivec4;
typedef Vector<unsigned int, 4> uvec4;

typedef Mat2<float>  mat2;
typedef Mat2<double> dmat2;
typedef Mat3<float>  mat3;
typedef Mat3<double> dmat3;
typedef Mat4<float>  mat4;
typedef Mat4<double> dmat4;

//== GENERAL MATRIX FUNCTIONS =================================================

/// matrix-matrix multiplication
template <typename Scalar, int M, int N, int K>
Matrix<Scalar, M, N> operator*(const Matrix<Scalar, M, K>& m1,
                               const Matrix<Scalar, K, N>& m2)
{
    Matrix<Scalar, M, N> m;
    int i, j, k;

    for (i = 0; i < M; ++i)
    {
        for (j = 0; j < N; ++j)
        {
            m(i, j) = Scalar(0);
            for (k = 0; k < K; ++k)
                m(i, j) += m1(i, k) * m2(k, j);
        }
    }

    return m;
}

//-----------------------------------------------------------------------------

template <typename Scalar, int M, int N>
Matrix<Scalar, N, M> transpose(const Matrix<Scalar, M, N>& m)
{
    Matrix<Scalar, N, M> result;

    for (int j = 0; j < M; ++j)
        for (int i = 0; i < N; ++i)
            result(i, j) = m(j, i);

    return result;
}

//-----------------------------------------------------------------------------

template <typename Scalar, int M, int N>
Matrix<Scalar, M, N>                  Matrix<Scalar, M, N>::identity()
{
    static_assert(M == N, "only for square matrices");

    Matrix<Scalar, N, N> m;

    for (int j = 0; j < N; ++j)
        for (int i = 0; i < N; ++i)
            m(i, j) = 0.0;

    for (int i = 0; i < N; ++i)
        m(i, i) = 1.0;

    return m;
}

//-----------------------------------------------------------------------------

//! matrix + matrix
template <typename Scalar, int M, int N>
inline Matrix<Scalar, M, N> operator+(const Matrix<Scalar, M, N>& m1,
                                      const Matrix<Scalar, M, N>& m2)
{
    return Matrix<Scalar, M, N>(m1) += m2;
}

//-----------------------------------------------------------------------------

//! matrix - matrix
template <typename Scalar, int M, int N>
inline Matrix<Scalar, M, N> operator-(const Matrix<Scalar, M, N>& m1,
                                      const Matrix<Scalar, M, N>& m2)
{
    return Matrix<Scalar, M, N>(m1) -= m2;
}

//-----------------------------------------------------------------------------

//! negate matrix
template <typename Scalar, int M, int N>
inline Matrix<Scalar, M, N> operator-(const Matrix<Scalar, M, N>& m)
{
    Matrix<Scalar, M, N> result;
    for (int i    = 0; i < result.size(); ++i)
        result[i] = -m[i];
    return result;
}

//-----------------------------------------------------------------------------

//! scalar * matrix
template <typename Scalar, typename Scalar2, int M, int N>
inline Matrix<Scalar, M, N> operator*(const Scalar2               s,
                                      const Matrix<Scalar, M, N>& m)
{
    return Matrix<Scalar, M, N>(m) *= s;
}

//-----------------------------------------------------------------------------

//! matrix * scalar
template <typename Scalar, typename Scalar2, int M, int N>
inline Matrix<Scalar, M, N> operator*(const Matrix<Scalar, M, N>& m,
                                      const Scalar2 s)
{
    return Matrix<Scalar, M, N>(m) *= s;
}

//-----------------------------------------------------------------------------

//! matrix / scalar
template <typename Scalar, typename Scalar2, int M, int N>
inline Matrix<Scalar, M, N> operator/(const Matrix<Scalar, M, N>& m,
                                      const Scalar2 s)
{
    return Matrix<Scalar, M, N>(m) /= s;
}

//-----------------------------------------------------------------------------

//! compute the Frobenius norm of a matrix (or Euclidean norm of a vector)
template <typename Scalar, int M, int N>
inline Scalar norm(const Matrix<Scalar, M, N>& m)
{
    return sqrt(sqrnorm(m));
}

//-----------------------------------------------------------------------------

//! compute the squared Frobenius norm of a matrix (or squared Euclidean norm of a vector)
template <typename Scalar, int M, int N>
inline Scalar sqrnorm(const Matrix<Scalar, M, N>& m)
{
    Scalar s(0.0);
    for (int i = 0; i < m.size(); ++i)
        s += m[i] * m[i];
    return s;
}

//-----------------------------------------------------------------------------

//! return a normalized copy of a vector
template <typename Scalar, int M, int N>
inline Matrix<Scalar, M, N> normalize(const Matrix<Scalar, M, N>& m)
{
    Scalar n = norm(m);
    n        = (n > std::numeric_limits<Scalar>::min()) ? 1.0 / n : 0.0;
    return m * n;
}

//-----------------------------------------------------------------------------

//! return component-wise minimum
template <typename Scalar, int M, int N>
inline Matrix<Scalar, M, N> min(const Matrix<Scalar, M, N>& m1,
                                const Matrix<Scalar, M, N>& m2)
{
    Matrix<Scalar, M, N> result;
    for (int i    = 0; i < result.size(); ++i)
        result[i] = std::min(m1[i], m2[i]);
    return result;
}

//-----------------------------------------------------------------------------

//! return component-wise maximum
template <typename Scalar, int M, int N>
inline Matrix<Scalar, M, N> max(const Matrix<Scalar, M, N>& m1,
                                const Matrix<Scalar, M, N>& m2)
{
    Matrix<Scalar, M, N> result;
    for (int i    = 0; i < result.size(); ++i)
        result[i] = std::max(m1[i], m2[i]);
    return result;
}

//== Mat4 functions ===========================================================

template <typename Scalar>
Mat4<Scalar> viewportMatrix(Scalar l, Scalar b, Scalar w, Scalar h)
{
    Mat4<Scalar> m(Scalar(0));

    m(0, 0) = 0.5 * w;
    m(0, 3) = 0.5 * w + l;
    m(1, 1) = 0.5 * h;
    m(1, 3) = 0.5 * h + b;
    m(2, 2) = 0.5;
    m(2, 3) = 0.5;
    m(3, 3) = 1.0f;

    return m;
}

//-----------------------------------------------------------------------------

template <typename Scalar>
Mat4<Scalar> inverseViewportMatrix(Scalar l, Scalar b, Scalar w, Scalar h)
{
    Mat4<Scalar> m(Scalar(0));

    m(0, 0) = 2.0 / w;
    m(0, 3) = -1.0 - (l + l) / w;
    m(1, 1) = 2.0 / h;
    m(1, 3) = -1.0 - (b + b) / h;
    m(2, 2) = 2.0;
    m(2, 3) = -1.0;
    m(3, 3) = 1.0f;

    return m;
}

//-----------------------------------------------------------------------------

template <typename Scalar>
Mat4<Scalar> frustumMatrix(Scalar l, Scalar r, Scalar b, Scalar t, Scalar n,
                           Scalar f)
{
    Mat4<Scalar> m(Scalar(0));

    m(0, 0) = (n + n) / (r - l);
    m(0, 2) = (r + l) / (r - l);
    m(1, 1) = (n + n) / (t - b);
    m(1, 2) = (t + b) / (t - b);
    m(2, 2) = -(f + n) / (f - n);
    m(2, 3) = -f * (n + n) / (f - n);
    m(3, 2) = -1.0f;

    return m;
}

//-----------------------------------------------------------------------------

template <typename Scalar>
Mat4<Scalar> inverseFrustumMatrix(Scalar l, Scalar r, Scalar b, Scalar t,
                                  Scalar n, Scalar f)
{
    Mat4<Scalar> m(Scalar(0));

    const Scalar nn = n + n;

    m(0, 0) = (r - l) / nn;
    m(0, 3) = (r + l) / nn;
    m(1, 1) = (t - b) / nn;
    m(1, 3) = (t + b) / nn;
    m(2, 3) = -1.0;
    m(3, 2) = (n - f) / (nn * f);
    m(3, 3) = (n + f) / (nn * f);

    return m;
}

//-----------------------------------------------------------------------------

template <typename Scalar>
Mat4<Scalar> perspectiveMatrix(Scalar fovy, Scalar aspect, Scalar near,
                               Scalar far)
{
    Scalar t = Scalar(near) * tan(fovy * M_PI / 360.0);
    Scalar b = -t;
    Scalar l = b * aspect;
    Scalar r = t * aspect;

    return frustumMatrix(l, r, b, t, Scalar(near), Scalar(far));
}

//-----------------------------------------------------------------------------

template <typename Scalar>
Mat4<Scalar> inversePerspectiveMatrix(Scalar fovy, Scalar aspect, Scalar near,
                                      Scalar far)
{
    Scalar t = near * tan(fovy * M_PI / 360.0);
    Scalar b = -t;
    Scalar l = b * aspect;
    Scalar r = t * aspect;

    return inverseFrustumMatrix(l, r, b, t, near, far);
}

//-----------------------------------------------------------------------------

template <typename Scalar>
Mat4<Scalar> orthoMatrix(Scalar left, Scalar right, Scalar bottom, Scalar top,
                         Scalar zNear, Scalar zFar)
{
    Mat4<Scalar> m(0.0);

    m(0, 0) = Scalar(2) / (right - left);
    m(1, 1) = Scalar(2) / (top - bottom);
    m(2, 2) = -Scalar(2) / (zFar - zNear);
    m(0, 3) = -(right + left) / (right - left);
    m(1, 3) = -(top + bottom) / (top - bottom);
    m(2, 3) = -(zFar + zNear) / (zFar - zNear);
    m(3, 3) = Scalar(1);

    return m;
}

//-----------------------------------------------------------------------------

template <typename Scalar>
Mat4<Scalar> lookAtMatrix(const Vector<Scalar, 3>& eye,
                          const Vector<Scalar, 3>& center,
                          const Vector<Scalar, 3>& up)
{

    Vector<Scalar, 3> z = normalize(eye - center);
    Vector<Scalar, 3> x = normalize(cross(up, z));
    Vector<Scalar, 3> y = normalize(cross(z, x));

    Mat4<Scalar> m;
    m(0, 0) = x[0];
    m(0, 1) = x[1];
    m(0, 2) = x[2];
    m(0, 3) = -dot(x, eye);
    m(1, 0) = y[0];
    m(1, 1) = y[1];
    m(1, 2) = y[2];
    m(1, 3) = -dot(y, eye);
    m(2, 0) = z[0];
    m(2, 1) = z[1];
    m(2, 2) = z[2];
    m(2, 3) = -dot(z, eye);
    m(3, 0) = 0.0;
    m(3, 1) = 0.0;
    m(3, 2) = 0.0;
    m(3, 3) = 1.0;

    return m;
}

//-----------------------------------------------------------------------------

template <typename Scalar>
Mat4<Scalar> translationMatrix(const Vector<Scalar, 3>& t)
{
    Mat4<Scalar> m(Scalar(0));
    m(0, 0) = m(1, 1) = m(2, 2) = m(3, 3) = 1.0f;
    m(0, 3) = t[0];
    m(1, 3) = t[1];
    m(2, 3) = t[2];

    return m;
}

//-----------------------------------------------------------------------------

template <typename Scalar>
Mat4<Scalar> rotationMatrixX(Scalar angle)
{
    Scalar ca = cos(angle * (M_PI / 180.0));
    Scalar sa = sin(angle * (M_PI / 180.0));

    Mat4<Scalar> m(0.0);
    m(0, 0) = 1.0;
    m(1, 1) = ca;
    m(1, 2) = -sa;
    m(2, 2) = ca;
    m(2, 1) = sa;
    m(3, 3) = 1.0;

    return m;
}

//-----------------------------------------------------------------------------

template <typename Scalar>
Mat4<Scalar> rotationMatrixY(Scalar angle)
{
    Scalar ca = cos(angle * (M_PI / 180.0));
    Scalar sa = sin(angle * (M_PI / 180.0));

    Mat4<Scalar> m(0.0);
    m(0, 0) = ca;
    m(0, 2) = sa;
    m(1, 1) = 1.0;
    m(2, 0) = -sa;
    m(2, 2) = ca;
    m(3, 3) = 1.0;

    return m;
}

//-----------------------------------------------------------------------------

template <typename Scalar>
Mat4<Scalar> rotationMatrixZ(Scalar angle)
{
    Scalar ca = cos(angle * (M_PI / 180.0));
    Scalar sa = sin(angle * (M_PI / 180.0));

    Mat4<Scalar> m(0.0);
    m(0, 0) = ca;
    m(0, 1) = -sa;
    m(1, 0) = sa;
    m(1, 1) = ca;
    m(2, 2) = 1.0;
    m(3, 3) = 1.0;

    return m;
}

//-----------------------------------------------------------------------------

template <typename Scalar>
Mat4<Scalar> rotationMatrix(const Vector<Scalar, 3>& axis, Scalar angle)
{
    Mat4<Scalar> m(Scalar(0));
    Scalar       a       = angle * (M_PI / 180.0f);
    Scalar       c       = cosf(a);
    Scalar       s       = sinf(a);
    Scalar       one_m_c = Scalar(1) - c;
    Vector<Scalar, 3> ax = normalize(axis);

    m(0, 0) = ax[0] * ax[0] * one_m_c + c;
    m(0, 1) = ax[0] * ax[1] * one_m_c - ax[2] * s;
    m(0, 2) = ax[0] * ax[2] * one_m_c + ax[1] * s;

    m(1, 0) = ax[1] * ax[0] * one_m_c + ax[2] * s;
    m(1, 1) = ax[1] * ax[1] * one_m_c + c;
    m(1, 2) = ax[1] * ax[2] * one_m_c - ax[0] * s;

    m(2, 0) = ax[2] * ax[0] * one_m_c - ax[1] * s;
    m(2, 1) = ax[2] * ax[1] * one_m_c + ax[0] * s;
    m(2, 2) = ax[2] * ax[2] * one_m_c + c;

    m(3, 3) = 1.0f;

    return m;
}

//-----------------------------------------------------------------------------

template <typename Scalar>
Mat3<Scalar> linearPart(const Mat4<Scalar>& m)
{
    Mat3<Scalar> result;
    for (int j = 0; j < 3; ++j)
        for (int i = 0; i < 3; ++i)
            result(i, j) = m(i, j);
    return result;
}

//-----------------------------------------------------------------------------

template <typename Scalar>
Vector<Scalar, 3> projectiveTransform(const Mat4<Scalar>&      m,
                                      const Vector<Scalar, 3>& v)
{
    const Scalar x = m(0, 0) * v[0] + m(0, 1) * v[1] + m(0, 2) * v[2] + m(0, 3);
    const Scalar y = m(1, 0) * v[0] + m(1, 1) * v[1] + m(1, 2) * v[2] + m(1, 3);
    const Scalar z = m(2, 0) * v[0] + m(2, 1) * v[1] + m(2, 2) * v[2] + m(2, 3);
    const Scalar w = m(3, 0) * v[0] + m(3, 1) * v[1] + m(3, 2) * v[2] + m(3, 3);
    return Vector<Scalar, 3>(x / w, y / w, z / w);
}

//-----------------------------------------------------------------------------

template <typename Scalar>
Vector<Scalar, 3> affineTransform(const Mat4<Scalar>&      m,
                                  const Vector<Scalar, 3>& v)
{
    const Scalar x = m(0, 0) * v[0] + m(0, 1) * v[1] + m(0, 2) * v[2] + m(0, 3);
    const Scalar y = m(1, 0) * v[0] + m(1, 1) * v[1] + m(1, 2) * v[2] + m(1, 3);
    const Scalar z = m(2, 0) * v[0] + m(2, 1) * v[1] + m(2, 2) * v[2] + m(2, 3);
    return Vector<Scalar, 3>(x, y, z);
}

//-----------------------------------------------------------------------------

template <typename Scalar>
Vector<Scalar, 3> linearTransform(const Mat4<Scalar>&      m,
                                  const Vector<Scalar, 3>& v)
{
    const Scalar x = m(0, 0) * v[0] + m(0, 1) * v[1] + m(0, 2) * v[2];
    const Scalar y = m(1, 0) * v[0] + m(1, 1) * v[1] + m(1, 2) * v[2];
    const Scalar z = m(2, 0) * v[0] + m(2, 1) * v[1] + m(2, 2) * v[2];
    return Vector<Scalar, 3>(x, y, z);
}

//-----------------------------------------------------------------------------

template <typename Scalar>
Mat4<Scalar> inverse(const Mat4<Scalar>& m)
{
    Scalar Coef00 = m(2, 2) * m(3, 3) - m(2, 3) * m(3, 2);
    Scalar Coef02 = m(2, 1) * m(3, 3) - m(2, 3) * m(3, 1);
    Scalar Coef03 = m(2, 1) * m(3, 2) - m(2, 2) * m(3, 1);

    Scalar Coef04 = m(1, 2) * m(3, 3) - m(1, 3) * m(3, 2);
    Scalar Coef06 = m(1, 1) * m(3, 3) - m(1, 3) * m(3, 1);
    Scalar Coef07 = m(1, 1) * m(3, 2) - m(1, 2) * m(3, 1);

    Scalar Coef08 = m(1, 2) * m(2, 3) - m(1, 3) * m(2, 2);
    Scalar Coef10 = m(1, 1) * m(2, 3) - m(1, 3) * m(2, 1);
    Scalar Coef11 = m(1, 1) * m(2, 2) - m(1, 2) * m(2, 1);

    Scalar Coef12 = m(0, 2) * m(3, 3) - m(0, 3) * m(3, 2);
    Scalar Coef14 = m(0, 1) * m(3, 3) - m(0, 3) * m(3, 1);
    Scalar Coef15 = m(0, 1) * m(3, 2) - m(0, 2) * m(3, 1);

    Scalar Coef16 = m(0, 2) * m(2, 3) - m(0, 3) * m(2, 2);
    Scalar Coef18 = m(0, 1) * m(2, 3) - m(0, 3) * m(2, 1);
    Scalar Coef19 = m(0, 1) * m(2, 2) - m(0, 2) * m(2, 1);

    Scalar Coef20 = m(0, 2) * m(1, 3) - m(0, 3) * m(1, 2);
    Scalar Coef22 = m(0, 1) * m(1, 3) - m(0, 3) * m(1, 1);
    Scalar Coef23 = m(0, 1) * m(1, 2) - m(0, 2) * m(1, 1);

    Vector<Scalar, 4> const SignA(+1, -1, +1, -1);
    Vector<Scalar, 4> const SignB(-1, +1, -1, +1);

    Vector<Scalar, 4> Fac0(Coef00, Coef00, Coef02, Coef03);
    Vector<Scalar, 4> Fac1(Coef04, Coef04, Coef06, Coef07);
    Vector<Scalar, 4> Fac2(Coef08, Coef08, Coef10, Coef11);
    Vector<Scalar, 4> Fac3(Coef12, Coef12, Coef14, Coef15);
    Vector<Scalar, 4> Fac4(Coef16, Coef16, Coef18, Coef19);
    Vector<Scalar, 4> Fac5(Coef20, Coef20, Coef22, Coef23);

    Vector<Scalar, 4> Vec0(m(0, 1), m(0, 0), m(0, 0), m(0, 0));
    Vector<Scalar, 4> Vec1(m(1, 1), m(1, 0), m(1, 0), m(1, 0));
    Vector<Scalar, 4> Vec2(m(2, 1), m(2, 0), m(2, 0), m(2, 0));
    Vector<Scalar, 4> Vec3(m(3, 1), m(3, 0), m(3, 0), m(3, 0));

    Vector<Scalar, 4> Inv0 = SignA * (Vec1 * Fac0 - Vec2 * Fac1 + Vec3 * Fac2);
    Vector<Scalar, 4> Inv1 = SignB * (Vec0 * Fac0 - Vec2 * Fac3 + Vec3 * Fac4);
    Vector<Scalar, 4> Inv2 = SignA * (Vec0 * Fac1 - Vec1 * Fac3 + Vec3 * Fac5);
    Vector<Scalar, 4> Inv3 = SignB * (Vec0 * Fac2 - Vec1 * Fac4 + Vec2 * Fac5);

    Mat4<Scalar> Inverse(Inv0, Inv1, Inv2, Inv3);

    Vector<Scalar, 4> Row0(Inverse(0, 0), Inverse(1, 0), Inverse(2, 0),
                           Inverse(3, 0));
    Vector<Scalar, 4> Col0(m(0, 0), m(0, 1), m(0, 2), m(0, 3));

    Scalar Determinant = dot(Col0, Row0);

    Inverse /= Determinant;

    return Inverse;
}

//== Mat3 functions ===========================================================

template <typename Scalar>
Mat3<Scalar> inverse(const Mat3<Scalar>& m)
{
    Scalar det = (-m(0, 0) * m(1, 1) * m(2, 2) + m(0, 0) * m(1, 2) * m(2, 1) +
                  m(1, 0) * m(0, 1) * m(2, 2) - m(1, 0) * m(0, 2) * m(2, 1) -
                  m(2, 0) * m(0, 1) * m(1, 2) + m(2, 0) * m(0, 2) * m(1, 1));

    Mat3<Scalar> inv;
    inv(0, 0) = (m(1, 2) * m(2, 1) - m(1, 1) * m(2, 2)) / det;
    inv(0, 1) = (m(0, 1) * m(2, 2) - m(0, 2) * m(2, 1)) / det;
    inv(0, 2) = (m(0, 2) * m(1, 1) - m(0, 1) * m(1, 2)) / det;
    inv(1, 0) = (m(1, 0) * m(2, 2) - m(1, 2) * m(2, 0)) / det;
    inv(1, 1) = (m(0, 2) * m(2, 0) - m(0, 0) * m(2, 2)) / det;
    inv(1, 2) = (m(0, 0) * m(1, 2) - m(0, 2) * m(1, 0)) / det;
    inv(2, 0) = (m(1, 1) * m(2, 0) - m(1, 0) * m(2, 1)) / det;
    inv(2, 1) = (m(0, 0) * m(2, 1) - m(0, 1) * m(2, 0)) / det;
    inv(2, 2) = (m(0, 1) * m(1, 0) - m(0, 0) * m(1, 1)) / det;

    return inv;
}

//-----------------------------------------------------------------------------

template <typename Scalar>
bool symmetricEigendecomposition(const Mat3<Scalar>& m, Scalar& eval1,
                                 Scalar& eval2, Scalar& eval3,
                                 Vector<Scalar, 3>&     evec1,
                                 Vector<Scalar, 3>&     evec2,
                                 Vector<Scalar, 3>&     evec3)
{
    unsigned int i, j;
    Scalar       theta, t, c, s;
    Mat3<Scalar> V = Mat3<Scalar>::identity();
    Mat3<Scalar> R;
    Mat3<Scalar> A   = m;
    const Scalar eps = 1e-10; //0.000001;

    int iterations = 100;
    while (iterations--)
    {
        // find largest off-diagonal elem
        if (fabs(A(0, 1)) < fabs(A(0, 2)))
        {
            if (fabs(A(0, 2)) < fabs(A(1, 2)))
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
            if (fabs(A(0, 1)) < fabs(A(1, 2)))
            {
                i = 1, j = 2;
            }
            else
            {
                i = 0, j = 1;
            }
        }

        // converged?
        if (fabs(A(i, j)) < eps)
            break;

        // compute Jacobi-Rotation
        theta = 0.5 * (A(j, j) - A(i, i)) / A(i, j);
        t     = 1.0 / (fabs(theta) + sqrt(1.0 + theta * theta));
        if (theta < 0.0)
            t = -t;

        c = 1.0 / sqrt(1.0 + t * t);
        s = t * c;

        R = Mat3<Scalar>::identity();
        R(i, i) = R(j, j) = c;
        R(i, j) = s;
        R(j, i) = -s;

        A = transpose(R) * A * R;
        V = V * R;
    }

    if (iterations > 0)
    {

        // sort and return
        int    sorted[3];
        Scalar d[3] = {A(0, 0), A(1, 1), A(2, 2)};

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

        evec1 = Vector<Scalar, 3>(V(0, sorted[0]), V(1, sorted[0]),
                                  V(2, sorted[0]));
        evec2 = Vector<Scalar, 3>(V(0, sorted[1]), V(1, sorted[1]),
                                  V(2, sorted[1]));
        evec3 = normalize(cross(evec1, evec2));

        return true;
    }

    return false;
}

//== Vector functions =========================================================

//! read the space-separated components of a vector from a stream
template <typename Scalar, int N>
inline std::istream& operator>>(std::istream& is, Vector<Scalar, N>& vec)
{
    for (int i = 0; i < N; ++i)
        is >> vec[i];
    return is;
}

//! output a vector by printing its space-separated compontens
template <typename Scalar, int N>
inline std::ostream& operator<<(std::ostream& os, const Vector<Scalar, N>& vec)
{
    for (int i = 0; i < N - 1; ++i)
        os << vec[i] << " ";
    os << vec[N - 1];
    return os;
}

//! compute the dot product of two vectors
template <typename Scalar, int N>
inline Scalar dot(const Vector<Scalar, N>& v0, const Vector<Scalar, N>& v1)
{
    Scalar p = v0[0] * v1[0];
    for (int i = 1; i < N; ++i)
        p += v0[i] * v1[i];
    return p;
}

//! compute the Euclidean distance between two points
template <typename Scalar, int N>
inline Scalar distance(const Vector<Scalar, N>& v0, const Vector<Scalar, N>& v1)
{
    Scalar dist(0), d;
    for (int i = 0; i < N; ++i)
    {
        d = v0[i] - v1[i];
        d *= d;
        dist += d;
    }
    return (Scalar)sqrt(dist);
}

//! compute the cross product of two vectors (only valid for 3D vectors)
template <typename Scalar>
inline Vector<Scalar, 3> cross(const Vector<Scalar, 3>& v0,
                               const Vector<Scalar, 3>& v1)
{
    return Vector<Scalar, 3>(v0[1] * v1[2] - v0[2] * v1[1],
                             v0[2] * v1[0] - v0[0] * v1[2],
                             v0[0] * v1[1] - v0[1] * v1[0]);
}

//=============================================================================
} // namespace
//=============================================================================
