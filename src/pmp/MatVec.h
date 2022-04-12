// Copyright 2011-2021 the Polygon Mesh Processing Library developers.
// Copyright 2001-2005 by Computer Graphics Group, RWTH Aachen
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include <cmath>
#include <cassert>

#include <iostream>
#include <limits>
#include <initializer_list>

#include <Eigen/Dense>

#include "pmp/Exceptions.h"

namespace pmp {

//! \addtogroup core
//!@{

//! Base class for MxN matrix
template <typename Scalar, int M, int N>
class Matrix
{
public:
    //! the scalar type of the vector
    using value_type = Scalar;

    //! returns number of rows of the matrix
    static constexpr int rows() { return M; }
    //! returns number of columns of the matrix
    static constexpr int cols() { return N; }
    //! returns the dimension of the vector (or size of the matrix, rows*cols)
    static constexpr int size() { return M * N; }

    //! default constructor
    Matrix() = default;

    //! construct with all entries being a given scalar (matrix and vector)
    explicit Matrix(Scalar s)
    {
        for (int i = 0; i < size(); ++i)
            data_[i] = s;
    }

    //! constructor with row-wise initializer list of M*N entries
    explicit Matrix(const std::initializer_list<Scalar>&& values)
    {
        assert(values.size() == M * N);
        int i = 0;
        for (Scalar v : values)
        {
            // convert row-wise initializer list to
            // column-wise matrix storage
            data_[M * (i % N) + i / N] = v;
            ++i;
        }
    }

    //! constructor with N column vectors of dimension M
    explicit Matrix(const std::initializer_list<Matrix<Scalar, M, 1>>&& columns)
    {
        assert(columns.size() == N);
        int j{};
        for (const Matrix<Scalar, M, 1>& v : columns)
        {
            for (int i = 0; i < M; ++i)
                data_[M * j + i] = v[i];
            ++j;
        }
    }

    //! constructor for 2D vectors
    explicit Matrix(Scalar x, Scalar y)
    {
        static_assert(M == 2 && N == 1, "only for 2D vectors");
        data_[0] = x;
        data_[1] = y;
    }

    //! constructor for 3D vectors
    explicit Matrix(Scalar x, Scalar y, Scalar z)
    {
        static_assert(M == 3 && N == 1, "only for 3D vectors");
        data_[0] = x;
        data_[1] = y;
        data_[2] = z;
    }

    //! constructor for 4D vectors
    explicit Matrix(Scalar x, Scalar y, Scalar z, Scalar w)
    {
        static_assert(M == 4 && N == 1, "only for 4D vectors");
        data_[0] = x;
        data_[1] = y;
        data_[2] = z;
        data_[3] = w;
    }

    //! constructor for 4D vectors
    explicit Matrix(Matrix<Scalar, 3, 1> xyz, Scalar w)
    {
        static_assert(M == 4 && N == 1, "only for 4D vectors");
        data_[0] = xyz[0];
        data_[1] = xyz[1];
        data_[2] = xyz[2];
        data_[3] = w;
    }

    //! construct 3x3 matrix from 3 column vectors
    // clang-format off
    Matrix(Matrix<Scalar, 3, 1> c0,
           Matrix<Scalar, 3, 1> c1,
           Matrix<Scalar, 3, 1> c2)
    {
        static_assert(M == 3 && N == 3, "only for 3x3 matrices");
        (*this)(0,0) = c0[0]; (*this)(0,1) = c1[0]; (*this)(0,2) = c2[0];
        (*this)(1,0) = c0[1]; (*this)(1,1) = c1[1]; (*this)(1,2) = c2[1];
        (*this)(2,0) = c0[2]; (*this)(2,1) = c1[2]; (*this)(2,2) = c2[2];
    }
    // clang-format on

    //! construct 4x4 matrix from 4 column vectors
    // clang-format off
    Matrix(Matrix<Scalar, 4, 1> c0,
           Matrix<Scalar, 4, 1> c1,
           Matrix<Scalar, 4, 1> c2,
           Matrix<Scalar, 4, 1> c3)
    {
        static_assert(M == 4 && N == 4, "only for 4x4 matrices");
        (*this)(0,0) = c0[0]; (*this)(0,1) = c1[0]; (*this)(0,2) = c2[0]; (*this)(0,3) = c3[0];
        (*this)(1,0) = c0[1]; (*this)(1,1) = c1[1]; (*this)(1,2) = c2[1]; (*this)(1,3) = c3[1];
        (*this)(2,0) = c0[2]; (*this)(2,1) = c1[2]; (*this)(2,2) = c2[2]; (*this)(2,3) = c3[2];
        (*this)(3,0) = c0[3]; (*this)(3,1) = c1[3]; (*this)(3,2) = c2[3]; (*this)(3,3) = c3[3];
    }
    // clang-format on

    //! construct 4x4 matrix from 16 (row-wise) entries
    // clang-format off
    Matrix(Scalar m00, Scalar m01, Scalar m02, Scalar m03,
           Scalar m10, Scalar m11, Scalar m12, Scalar m13,
           Scalar m20, Scalar m21, Scalar m22, Scalar m23,
           Scalar m30, Scalar m31, Scalar m32, Scalar m33)
    {
        static_assert(M == 4 && N == 4, "only for 4x4 matrices");
        (*this)(0,0) = m00; (*this)(0,1) = m01; (*this)(0,2) = m02; (*this)(0,3) = m03;
        (*this)(1,0) = m10; (*this)(1,1) = m11; (*this)(1,2) = m12; (*this)(1,3) = m13;
        (*this)(2,0) = m20; (*this)(2,1) = m21; (*this)(2,2) = m22; (*this)(2,3) = m23;
        (*this)(3,0) = m30; (*this)(3,1) = m31; (*this)(3,2) = m32; (*this)(3,3) = m33;
    }
    // clang-format on

    //! construct from Eigen
    template <typename Derived>
    Matrix(const Eigen::MatrixBase<Derived>& m)
    {
        // don't distinguish between row and column vectors
        if (m.rows() == 1 || m.cols() == 1)
        {
            assert(m.size() == size());
            for (int i = 0; i < size(); ++i)
                (*this)[i] = m(i);
        }
        else
        {
            assert(m.rows() == rows() && m.cols() == cols());
            for (int i = 0; i < rows(); ++i)
                for (int j = 0; j < cols(); ++j)
                    (*this)(i, j) = m(i, j);
        }
    }

    //! copy constructor from other scalar type
    //! is also invoked for type-casting
    template <typename OtherScalarType>
    explicit Matrix(const Matrix<OtherScalarType, M, N>& m)
    {
        for (int i = 0; i < size(); ++i)
            data_[i] = static_cast<Scalar>(m[i]);
    }

    //! assign from Eigen
    template <typename Derived>
    Matrix<Scalar, M, N>& operator=(const Eigen::MatrixBase<Derived>& m)
    {
        // don't distinguish between row and column vectors
        if (m.rows() == 1 || m.cols() == 1)
        {
            assert(m.size() == size());
            for (int i = 0; i < size(); ++i)
                (*this)[i] = static_cast<Scalar>(m(i));
        }
        else
        {
            assert(m.rows() == rows() && m.cols() == cols());
            for (int i = 0; i < rows(); ++i)
                for (int j = 0; j < cols(); ++j)
                    (*this)(i, j) = static_cast<Scalar>(m(i, j));
        }
        return *this;
    }

    //! cast to Eigen
    template <typename OtherScalar>
    operator Eigen::Matrix<OtherScalar, M, N>() const
    {
        Eigen::Matrix<OtherScalar, M, N> m;
        for (int i = 0; i < rows(); ++i)
            for (int j = 0; j < cols(); ++j)
                m(i, j) = static_cast<OtherScalar>((*this)(i, j));
        return m;
    }

    //! return identity matrix (only for square matrices, N==M)
    static Matrix<Scalar, M, N> identity();

    //! access entry at row i and column j
    Scalar& operator()(unsigned int i, unsigned int j)
    {
        assert(i < M && j < N);
        return data_[M * j + i];
    }

    //! const-access entry at row i and column j
    const Scalar& operator()(unsigned int i, unsigned int j) const
    {
        assert(i < M && j < N);
        return data_[M * j + i];
    }

    //! access i'th entry (use for vectors)
    Scalar& operator[](unsigned int i)
    {
        assert(i < M * N);
        return data_[i];
    }

    //! const-access i'th entry (use for vectors)
    Scalar operator[](unsigned int i) const
    {
        assert(i < M * N);
        return data_[i];
    }

    //! const-access as scalar array
    const Scalar* data() const { return data_.data(); }

    //! access as scalar array
    Scalar* data() { return data_.data(); }

    //! normalize matrix/vector by dividing through Frobenius/Euclidean norm
    void normalize()
    {
        Scalar n = norm(*this);
        n = (n > std::numeric_limits<Scalar>::min()) ? Scalar(1.0) / n
                                                     : Scalar(0.0);
        *this *= n;
    }

    //! divide matrix by scalar
    Matrix<Scalar, M, N>& operator/=(const Scalar s)
    {
        for (int i = 0; i < size(); ++i)
            data_[i] /= s;
        return *this;
    }

    //! multply matrix by scalar
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

    //! subtract other matrix from this matrix
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

    //! \return true if all elements are finite, i.e. not NaN or +/- inf
    bool allFinite() const
    {
        for (int i = 0; i < size(); ++i)
            if (!std::isfinite(data_[i]))
                return false;
        return true;
    }

protected:
    std::array<Scalar, N * M> data_;
};

//! template specialization for Vector as Nx1 matrix
template <typename Scalar, int M>
using Vector = Matrix<Scalar, M, 1>;

//! template specialization for 4x4 matrices
template <typename Scalar>
using Mat4 = Matrix<Scalar, 4, 4>;

//! template specialization for 3x3 matrices
template <typename Scalar>
using Mat3 = Matrix<Scalar, 3, 3>;

//! template specialization for 2x2 matrices
template <typename Scalar>
using Mat2 = Matrix<Scalar, 2, 2>;

//! template specialization for a vector of two float values
using vec2 = Vector<float, 2>;
//! template specialization for a vector of two double values
using dvec2 = Vector<double, 2>;
//! template specialization for a vector of two bool values
using bvec2 = Vector<bool, 2>;
//! template specialization for a vector of two int values
using ivec2 = Vector<int, 2>;
//! template specialization for a vector of two unsigned int values
using uvec2 = Vector<unsigned int, 2>;

//! template specialization for a vector of three float values
using vec3 = Vector<float, 3>;
//! template specialization for a vector of three double values
using dvec3 = Vector<double, 3>;
//! template specialization for a vector of three bool values
using bvec3 = Vector<bool, 3>;
//! template specialization for a vector of three int values
using ivec3 = Vector<int, 3>;
//! template specialization for a vector of three unsigned int values
using uvec3 = Vector<unsigned int, 3>;

//! template specialization for a vector of four float values
using vec4 = Vector<float, 4>;
//! template specialization for a vector of four double values
using dvec4 = Vector<double, 4>;
//! template specialization for a vector of four bool values
using bvec4 = Vector<bool, 4>;
//! template specialization for a vector of four int values
using ivec4 = Vector<int, 4>;
//! template specialization for a vector of four unsigned int values
using uvec4 = Vector<unsigned int, 4>;

//! template specialization for a 2x2 matrix of float values
using mat2 = Mat2<float>;
//! template specialization for a 2x2 matrix of double values
using dmat2 = Mat2<double>;
//! template specialization for a 3x3 matrix of float values
using mat3 = Mat3<float>;
//! template specialization for a 3x3 matrix of double values
using dmat3 = Mat3<double>;
//! template specialization for a 4x4 matrix of float values
using mat4 = Mat4<float>;
//! template specialization for a 4x4 matrix of double values
using dmat4 = Mat4<double>;

//! output a matrix by printing its space-separated compontens
template <typename Scalar, int M, int N>
inline std::ostream& operator<<(std::ostream& os, const Matrix<Scalar, M, N>& m)
{
    for (int i = 0; i < M; ++i)
    {
        for (int j = 0; j < N; ++j)
            os << m(i, j) << " ";
        os << std::endl;
    }
    return os;
}

//! matrix-matrix multiplication
template <typename Scalar, int M, int N, int K>
Matrix<Scalar, M, N> operator*(const Matrix<Scalar, M, K>& m1,
                               const Matrix<Scalar, K, N>& m2)
{
    Matrix<Scalar, M, N> m;

    for (int i = 0; i < M; ++i)
    {
        for (int j = 0; j < N; ++j)
        {
            m(i, j) = Scalar(0);
            for (int k = 0; k < K; ++k)
                m(i, j) += m1(i, k) * m2(k, j);
        }
    }

    return m;
}

//! component-wise multiplication
template <typename Scalar, int M, int N>
Matrix<Scalar, M, N> cmult(const Matrix<Scalar, M, N>& m1,
                           const Matrix<Scalar, M, N>& m2)
{
    Matrix<Scalar, M, N> m;

    for (int i = 0; i < M; ++i)
        for (int j = 0; j < N; ++j)
            m(i, j) = m1(i, j) * m2(i, j);

    return m;
}

//! transpose MxN matrix to NxM matrix
template <typename Scalar, int M, int N>
Matrix<Scalar, N, M> transpose(const Matrix<Scalar, M, N>& m)
{
    Matrix<Scalar, N, M> result;

    for (int j = 0; j < M; ++j)
        for (int i = 0; i < N; ++i)
            result(i, j) = m(j, i);

    return result;
}

//! identity matrix (work only for square matrices)
template <typename Scalar, int M, int N>
Matrix<Scalar, M, N> Matrix<Scalar, M, N>::identity()
{
    static_assert(M == N, "only for square matrices");

    Matrix<Scalar, M, M> m;

    for (int j = 0; j < M; ++j)
        for (int i = 0; i < M; ++i)
            m(i, j) = 0.0;

    for (int i = 0; i < M; ++i)
        m(i, i) = 1.0;

    return m;
}

//! matrix addition: m1 + m2
template <typename Scalar, int M, int N>
inline Matrix<Scalar, M, N> operator+(const Matrix<Scalar, M, N>& m1,
                                      const Matrix<Scalar, M, N>& m2)
{
    return Matrix<Scalar, M, N>(m1) += m2;
}

//! matrix subtraction: m1 - m2
template <typename Scalar, int M, int N>
inline Matrix<Scalar, M, N> operator-(const Matrix<Scalar, M, N>& m1,
                                      const Matrix<Scalar, M, N>& m2)
{
    return Matrix<Scalar, M, N>(m1) -= m2;
}

//! matrix negation: -m
template <typename Scalar, int M, int N>
inline Matrix<Scalar, M, N> operator-(const Matrix<Scalar, M, N>& m)
{
    Matrix<Scalar, M, N> result;
    for (int i = 0; i < result.size(); ++i)
        result[i] = -m[i];
    return result;
}

//! scalar multiplication of matrix: s*m
template <typename Scalar, typename Scalar2, int M, int N>
inline Matrix<Scalar, M, N> operator*(const Scalar2 s,
                                      const Matrix<Scalar, M, N>& m)
{
    return Matrix<Scalar, M, N>(m) *= static_cast<Scalar>(s);
}

//! scalar multiplication of matrix: m*s
template <typename Scalar, typename Scalar2, int M, int N>
inline Matrix<Scalar, M, N> operator*(const Matrix<Scalar, M, N>& m,
                                      const Scalar2 s)
{
    return Matrix<Scalar, M, N>(m) *= static_cast<Scalar>(s);
}

//! divide matrix by scalar: m/s
template <typename Scalar, typename Scalar2, int M, int N>
inline Matrix<Scalar, M, N> operator/(const Matrix<Scalar, M, N>& m,
                                      const Scalar2 s)
{
    return Matrix<Scalar, M, N>(m) /= s;
}

//! compute the Frobenius norm of a matrix (or Euclidean norm of a vector)
template <typename Scalar, int M, int N>
inline Scalar norm(const Matrix<Scalar, M, N>& m)
{
    return sqrt(sqrnorm(m));
}

//! compute the squared Frobenius norm of a matrix (or squared Euclidean norm of a vector)
template <typename Scalar, int M, int N>
inline Scalar sqrnorm(const Matrix<Scalar, M, N>& m)
{
    Scalar s(0.0);
    for (int i = 0; i < m.size(); ++i)
        s += m[i] * m[i];
    return s;
}

//! return a normalized copy of a matrix or a vector
template <typename Scalar, int M, int N>
inline Matrix<Scalar, M, N> normalize(const Matrix<Scalar, M, N>& m)
{
    Scalar n = norm(m);
    n = (n > std::numeric_limits<Scalar>::min()) ? Scalar(1.0) / n
                                                 : Scalar(0.0);
    return m * n;
}

//! return component-wise minimum
template <typename Scalar, int M, int N>
inline Matrix<Scalar, M, N> min(const Matrix<Scalar, M, N>& m1,
                                const Matrix<Scalar, M, N>& m2)
{
    Matrix<Scalar, M, N> result;
    for (int i = 0; i < result.size(); ++i)
        result[i] = std::min(m1[i], m2[i]);
    return result;
}

//! return component-wise maximum
template <typename Scalar, int M, int N>
inline Matrix<Scalar, M, N> max(const Matrix<Scalar, M, N>& m1,
                                const Matrix<Scalar, M, N>& m2)
{
    Matrix<Scalar, M, N> result;
    for (int i = 0; i < result.size(); ++i)
        result[i] = std::max(m1[i], m2[i]);
    return result;
}

//! OpenGL viewport matrix with parameters left, bottom, width, height
template <typename Scalar>
Mat4<Scalar> viewport_matrix(Scalar l, Scalar b, Scalar w, Scalar h)
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

//! inverse of OpenGL viewport matrix with parameters left, bottom, width, height
//! \sa viewport_matrix
template <typename Scalar>
Mat4<Scalar> inverse_viewport_matrix(Scalar l, Scalar b, Scalar w, Scalar h)
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

//! OpenGL frustum matrix with parameters left, right, bottom, top, near, far
template <typename Scalar>
Mat4<Scalar> frustum_matrix(Scalar l, Scalar r, Scalar b, Scalar t, Scalar n,
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

//! inverse of OpenGL frustum matrix with parameters left, right, bottom, top, near, far
//! \sa frustum_matrix
template <typename Scalar>
Mat4<Scalar> inverse_frustum_matrix(Scalar l, Scalar r, Scalar b, Scalar t,
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

//! OpenGL perspective matrix with parameters field of view in y-direction,
//! aspect ratio, and distance of near and far planes
template <typename Scalar>
Mat4<Scalar> perspective_matrix(Scalar fovy, Scalar aspect, Scalar zNear,
                                Scalar zFar)
{
    Scalar t = Scalar(zNear) * tan(fovy * Scalar(M_PI / 360.0));
    Scalar b = -t;
    Scalar l = b * aspect;
    Scalar r = t * aspect;

    return frustum_matrix(l, r, b, t, Scalar(zNear), Scalar(zFar));
}

//! inverse of perspective matrix
//! \sa perspective_matrix
template <typename Scalar>
Mat4<Scalar> inverse_perspective_matrix(Scalar fovy, Scalar aspect,
                                        Scalar zNear, Scalar zFar)
{
    Scalar t = zNear * tan(fovy * Scalar(M_PI / 360.0));
    Scalar b = -t;
    Scalar l = b * aspect;
    Scalar r = t * aspect;

    return inverse_frustum_matrix(l, r, b, t, zNear, zFar);
}

//! OpenGL orthogonal projection matrix with parameters left, right, bottom,
//! top, near, far
template <typename Scalar>
Mat4<Scalar> ortho_matrix(Scalar left, Scalar right, Scalar bottom, Scalar top,
                          Scalar zNear = -1, Scalar zFar = 1)
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

//! OpenGL look-at camera matrix with parameters eye position, scene center, up-direction
template <typename Scalar>
Mat4<Scalar> look_at_matrix(const Vector<Scalar, 3>& eye,
                            const Vector<Scalar, 3>& center,
                            const Vector<Scalar, 3>& up)
{
    Vector<Scalar, 3> z = normalize(eye - center);
    Vector<Scalar, 3> x = normalize(cross(up, z));
    Vector<Scalar, 3> y = normalize(cross(z, x));

    // clang-format off
    Mat4<Scalar> m;
    m(0, 0) = x[0]; m(0, 1) = x[1]; m(0, 2) = x[2]; m(0, 3) = -dot(x, eye);
    m(1, 0) = y[0]; m(1, 1) = y[1]; m(1, 2) = y[2]; m(1, 3) = -dot(y, eye);
    m(2, 0) = z[0]; m(2, 1) = z[1]; m(2, 2) = z[2]; m(2, 3) = -dot(z, eye);
    m(3, 0) = 0.0;  m(3, 1) = 0.0;  m(3, 2) = 0.0;  m(3, 3) = 1.0;
    // clang-format on

    return m;
}

//! OpenGL matrix for translation by vector t
template <typename Scalar>
Mat4<Scalar> translation_matrix(const Vector<Scalar, 3>& t)
{
    Mat4<Scalar> m(Scalar(0));
    m(0, 0) = m(1, 1) = m(2, 2) = m(3, 3) = 1.0f;
    m(0, 3) = t[0];
    m(1, 3) = t[1];
    m(2, 3) = t[2];

    return m;
}

//! OpenGL matrix for scaling x/y/z by s
template <typename Scalar>
Mat4<Scalar> scaling_matrix(const Scalar s)
{
    Mat4<Scalar> m(Scalar(0));
    m(0, 0) = m(1, 1) = m(2, 2) = s;
    m(3, 3) = 1.0f;

    return m;
}

//! OpenGL matrix for scaling x/y/z by the components of s
template <typename Scalar>
Mat4<Scalar> scaling_matrix(const Vector<Scalar, 3>& s)
{
    Mat4<Scalar> m(Scalar(0));
    m(0, 0) = s[0];
    m(1, 1) = s[1];
    m(2, 2) = s[2];
    m(3, 3) = 1.0f;

    return m;
}

//! OpenGL matrix for rotation around x-axis by given angle (in degrees)
template <typename Scalar>
Mat4<Scalar> rotation_matrix_x(Scalar angle)
{
    Scalar ca = cos(angle * Scalar(M_PI / 180.0));
    Scalar sa = sin(angle * Scalar(M_PI / 180.0));

    Mat4<Scalar> m(0.0);
    m(0, 0) = 1.0;
    m(1, 1) = ca;
    m(1, 2) = -sa;
    m(2, 2) = ca;
    m(2, 1) = sa;
    m(3, 3) = 1.0;

    return m;
}

//! OpenGL matrix for rotation around y-axis by given angle (in degrees)
template <typename Scalar>
Mat4<Scalar> rotation_matrix_y(Scalar angle)
{
    Scalar ca = cos(angle * Scalar(M_PI / 180.0));
    Scalar sa = sin(angle * Scalar(M_PI / 180.0));

    Mat4<Scalar> m(0.0);
    m(0, 0) = ca;
    m(0, 2) = sa;
    m(1, 1) = 1.0;
    m(2, 0) = -sa;
    m(2, 2) = ca;
    m(3, 3) = 1.0;

    return m;
}

//! OpenGL matrix for rotation around z-axis by given angle (in degrees)
template <typename Scalar>
Mat4<Scalar> rotation_matrix_z(Scalar angle)
{
    Scalar ca = cos(angle * Scalar(M_PI / 180.0));
    Scalar sa = sin(angle * Scalar(M_PI / 180.0));

    Mat4<Scalar> m(0.0);
    m(0, 0) = ca;
    m(0, 1) = -sa;
    m(1, 0) = sa;
    m(1, 1) = ca;
    m(2, 2) = 1.0;
    m(3, 3) = 1.0;

    return m;
}

//! OpenGL matrix for rotation around given axis by given angle (in degrees)
template <typename Scalar>
Mat4<Scalar> rotation_matrix(const Vector<Scalar, 3>& axis, Scalar angle)
{
    Mat4<Scalar> m(Scalar(0));
    Scalar a = angle * Scalar(M_PI / 180.0f);
    Scalar c = cosf(a);
    Scalar s = sinf(a);
    Scalar one_m_c = Scalar(1) - c;
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

//! OpenGL matrix for rotation specified by unit quaternion
template <typename Scalar>
Mat4<Scalar> rotation_matrix(const Vector<Scalar, 4>& quat)
{
    Mat4<Scalar> m(0.0f);
    Scalar s1(1);
    Scalar s2(2);

    m(0, 0) = s1 - s2 * quat[1] * quat[1] - s2 * quat[2] * quat[2];
    m(1, 0) = s2 * quat[0] * quat[1] + s2 * quat[3] * quat[2];
    m(2, 0) = s2 * quat[0] * quat[2] - s2 * quat[3] * quat[1];

    m(0, 1) = s2 * quat[0] * quat[1] - s2 * quat[3] * quat[2];
    m(1, 1) = s1 - s2 * quat[0] * quat[0] - s2 * quat[2] * quat[2];
    m(2, 1) = s2 * quat[1] * quat[2] + s2 * quat[3] * quat[0];

    m(0, 2) = s2 * quat[0] * quat[2] + s2 * quat[3] * quat[1];
    m(1, 2) = s2 * quat[1] * quat[2] - s2 * quat[3] * quat[0];
    m(2, 2) = s1 - s2 * quat[0] * quat[0] - s2 * quat[1] * quat[1];

    m(3, 3) = 1.0f;

    return m;
}

//! return upper 3x3 matrix from given 4x4 matrix, corresponding to the
//! linear part of an affine transformation
template <typename Scalar>
Mat3<Scalar> linear_part(const Mat4<Scalar>& m)
{
    Mat3<Scalar> result;
    for (int j = 0; j < 3; ++j)
        for (int i = 0; i < 3; ++i)
            result(i, j) = m(i, j);
    return result;
}

//! projective transformation of 3D vector v by a 4x4 matrix m:
//! add 1 as 4th component of v, multiply m*v, divide by 4th component
template <typename Scalar>
Vector<Scalar, 3> projective_transform(const Mat4<Scalar>& m,
                                       const Vector<Scalar, 3>& v)
{
    const Scalar x = m(0, 0) * v[0] + m(0, 1) * v[1] + m(0, 2) * v[2] + m(0, 3);
    const Scalar y = m(1, 0) * v[0] + m(1, 1) * v[1] + m(1, 2) * v[2] + m(1, 3);
    const Scalar z = m(2, 0) * v[0] + m(2, 1) * v[1] + m(2, 2) * v[2] + m(2, 3);
    const Scalar w = m(3, 0) * v[0] + m(3, 1) * v[1] + m(3, 2) * v[2] + m(3, 3);
    return Vector<Scalar, 3>(x / w, y / w, z / w);
}

//! affine transformation of 3D vector v by a 4x4 matrix m:
//! add 1 as 4th component of v, multiply m*v, do NOT divide by 4th component
template <typename Scalar>
Vector<Scalar, 3> affine_transform(const Mat4<Scalar>& m,
                                   const Vector<Scalar, 3>& v)
{
    const Scalar x = m(0, 0) * v[0] + m(0, 1) * v[1] + m(0, 2) * v[2] + m(0, 3);
    const Scalar y = m(1, 0) * v[0] + m(1, 1) * v[1] + m(1, 2) * v[2] + m(1, 3);
    const Scalar z = m(2, 0) * v[0] + m(2, 1) * v[1] + m(2, 2) * v[2] + m(2, 3);
    return Vector<Scalar, 3>(x, y, z);
}

//! linear transformation of 3D vector v by a 4x4 matrix m:
//! transform vector by upper-left 3x3 submatrix of m
template <typename Scalar>
Vector<Scalar, 3> linear_transform(const Mat4<Scalar>& m,
                                   const Vector<Scalar, 3>& v)
{
    const Scalar x = m(0, 0) * v[0] + m(0, 1) * v[1] + m(0, 2) * v[2];
    const Scalar y = m(1, 0) * v[0] + m(1, 1) * v[1] + m(1, 2) * v[2];
    const Scalar z = m(2, 0) * v[0] + m(2, 1) * v[1] + m(2, 2) * v[2];
    return Vector<Scalar, 3>(x, y, z);
}

//! return the inverse of a 4x4 matrix
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

    // clang-format off
    Vector<Scalar, 4> Inv0 = cmult(SignA, (cmult(Vec1, Fac0) - cmult(Vec2, Fac1) + cmult(Vec3, Fac2)));
    Vector<Scalar, 4> Inv1 = cmult(SignB, (cmult(Vec0, Fac0) - cmult(Vec2, Fac3) + cmult(Vec3, Fac4)));
    Vector<Scalar, 4> Inv2 = cmult(SignA, (cmult(Vec0, Fac1) - cmult(Vec1, Fac3) + cmult(Vec3, Fac5)));
    Vector<Scalar, 4> Inv3 = cmult(SignB, (cmult(Vec0, Fac2) - cmult(Vec1, Fac4) + cmult(Vec2, Fac5)));
    // clang-format on

    Mat4<Scalar> Inverse(Inv0, Inv1, Inv2, Inv3);

    Vector<Scalar, 4> Row0(Inverse(0, 0), Inverse(1, 0), Inverse(2, 0),
                           Inverse(3, 0));
    Vector<Scalar, 4> Col0(m(0, 0), m(0, 1), m(0, 2), m(0, 3));

    Scalar Determinant = dot(Col0, Row0);

    Inverse /= Determinant;

    return Inverse;
}

//! return determinant of 3x3 matrix
template <typename Scalar>
Scalar determinant(const Mat3<Scalar>& m)
{
    return m(0, 0) * m(1, 1) * m(2, 2) - m(0, 0) * m(1, 2) * m(2, 1) +
           m(1, 0) * m(0, 2) * m(2, 1) - m(1, 0) * m(0, 1) * m(2, 2) +
           m(2, 0) * m(0, 1) * m(1, 2) - m(2, 0) * m(0, 2) * m(1, 1);
}

//! return the inverse of a 3x3 matrix
template <typename Scalar>
Mat3<Scalar> inverse(const Mat3<Scalar>& m)
{
    const Scalar det = determinant(m);
    if (det < 1.0e-10 || std::isnan(det))
    {
        throw SolverException("3x3 matrix not invertible");
    }

    Mat3<Scalar> inv;
    inv(0, 0) = (m(1, 1) * m(2, 2) - m(1, 2) * m(2, 1)) / det;
    inv(0, 1) = (m(0, 2) * m(2, 1) - m(0, 1) * m(2, 2)) / det;
    inv(0, 2) = (m(0, 1) * m(1, 2) - m(0, 2) * m(1, 1)) / det;
    inv(1, 0) = (m(1, 2) * m(2, 0) - m(1, 0) * m(2, 2)) / det;
    inv(1, 1) = (m(0, 0) * m(2, 2) - m(0, 2) * m(2, 0)) / det;
    inv(1, 2) = (m(0, 2) * m(1, 0) - m(0, 0) * m(1, 2)) / det;
    inv(2, 0) = (m(1, 0) * m(2, 1) - m(1, 1) * m(2, 0)) / det;
    inv(2, 1) = (m(0, 1) * m(2, 0) - m(0, 0) * m(2, 1)) / det;
    inv(2, 2) = (m(0, 0) * m(1, 1) - m(0, 1) * m(1, 0)) / det;

    return inv;
}

//! compute eigenvector/eigenvalue decomposition of a 3x3 matrix
template <typename Scalar>
bool symmetric_eigendecomposition(const Mat3<Scalar>& m, Scalar& eval1,
                                  Scalar& eval2, Scalar& eval3,
                                  Vector<Scalar, 3>& evec1,
                                  Vector<Scalar, 3>& evec2,
                                  Vector<Scalar, 3>& evec3)
{
    unsigned int i{}, j{};
    Scalar theta, t, c, s;
    Mat3<Scalar> V = Mat3<Scalar>::identity();
    Mat3<Scalar> R;
    Mat3<Scalar> A = m;
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
        t = 1.0 / (fabs(theta) + sqrt(1.0 + theta * theta));
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
        std::array<int, 3> sorted;
        std::array<Scalar, 3> d = {A(0, 0), A(1, 1), A(2, 2)};

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

//! compute perpendicular vector (rotate vector counter-clockwise by 90 degrees)
template <typename Scalar>
inline Vector<Scalar, 2> perp(const Vector<Scalar, 2>& v)
{
    return Vector<Scalar, 2>(-v[1], v[0]);
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

//!@}

} // namespace pmp
