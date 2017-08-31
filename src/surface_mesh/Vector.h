//=============================================================================
// Copyright (C) 2001-2005 by Computer Graphics Group, RWTH Aachen
// Copyright (C) 2011-2016 by Graphics & Geometry Group, Bielefeld University
// Copyright (C) 2017 Daniel Sieger
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

#include <cassert>
#include <cmath>
#include <cstring>
#include <iostream>
#include <limits>

//=============================================================================

#ifdef _WIN32
#undef min
#undef max
#endif

//=============================================================================

namespace surface_mesh {

//=============================================================================

//! A vector class for an N-dimensional vector of scalar type T.
//! Elements of a vector v can be accessed by v[0], v[1], ...
//! For 3D vectors one can also use v.x, v.y and v.z.
template <typename Scalar, int N>
class Vector
{
public:
    //! the scalar type of the vector
    typedef Scalar value_type;

    //! returns the dimension of the vector
    static int size() { return N; }

    //! default constructor, creates uninitialized values.
    Vector() {}

    //! construct from scalar s (fills all components with s)
    explicit Vector(const Scalar s)
    {
        for (int i   = 0; i < N; ++i)
            data_[i] = s;
    }

    //! construct from 2 scalars. only valid for 2D vectors.
    Vector(const Scalar x, const Scalar y)
    {
        assert(N == 2);
        data_[0] = x;
        data_[1] = y;
    }

    //! construct from 3 scalars. only valid for 2D vectors.
    Vector(const Scalar x, const Scalar y, const Scalar z)
    {
        assert(N == 3);
        data_[0] = x;
        data_[1] = y;
        data_[2] = z;
    }

    //! construct from 4 scalars. only valid for 2D vectors.
    Vector(const Scalar x, const Scalar y, const Scalar z, const Scalar w)
    {
        assert(N == 4);
        data_[0] = x;
        data_[1] = y;
        data_[2] = z;
        data_[3] = w;
    }

    //! construct from 3D Vector and 1 scalar. only valid for 4D vectors.
    Vector(Vector<Scalar, 3> v, const Scalar w)
    {
        assert(N == 4);
        data_[0] = v[0];
        data_[1] = v[1];
        data_[2] = v[2];
        data_[3] = w;
    }

    //! construct from vector of other scalar type
    template <typename OtherScalarType>
    explicit Vector(const Vector<OtherScalarType, N>& o)
    {
        for (int i   = 0; i < N; ++i)
            data_[i] = o.data_[i];
    }

    //! cast to vector of other scalar type
    template <typename OtherScalarType>
    operator Vector<OtherScalarType, N>()
    {
        Vector<OtherScalarType, N> v;
        for (int i = 0; i < N; ++i)
            v[i]   = data_[i];
        return v;
    }

    //! access to Scalar array
    Scalar* data() { return data_; }

    //! access to const Scalar array
    const Scalar* data() const { return data_; }

    //! get i'th element read-write
    Scalar& operator[](unsigned int i)
    {
        assert(i < N);
        return data_[i];
    }

    //! get i'th element read-only
    const Scalar operator[](unsigned int i) const
    {
        assert(i < N);
        return data_[i];
    }

    //! assign a scalar to all componenets
    Vector<Scalar, N>& operator=(const Scalar s)
    {
        for (int i   = 0; i < N; ++i)
            data_[i] = s;
        return *this;
    }

    //! assignment from a vector of different scalar type
    template <typename otherScalarType>
    Vector<Scalar, N>& operator=(const Vector<otherScalarType, N>& o)
    {
        for (int i   = 0; i < N; ++i)
            data_[i] = Scalar(o[i]);
        return *this;
    }

    //! component-wise comparison
    bool operator==(const Vector<Scalar, N>& other) const
    {
        for (int i = 0; i < N; ++i)
            if (data_[i] != other.data_[i])
                return false;
        return true;
    }

    //! component-wise comparison
    bool operator!=(const Vector<Scalar, N>& other) const
    {
        for (int i = 0; i < N; ++i)
            if (data_[i] != other.data_[i])
                return true;
        return false;
    }

    //! multiply vector by scalar s
    Vector<Scalar, N>& operator*=(const Scalar s)
    {
        for (int i = 0; i < N; ++i)
            data_[i] *= s;
        return *this;
    }

    //! divide vector by scalar s
    Vector<Scalar, N>& operator/=(const Scalar s)
    {
        for (int i = 0; i < N; ++i)
            data_[i] /= s;
        return *this;
    }

    //! component-wise multiplication by vector
    Vector<Scalar, N>& operator*=(const Vector<Scalar, N>& v)
    {
        for (int i = 0; i < N; ++i)
            data_[i] *= v.data_[i];
        return *this;
    }

    //! component-wise division by vector
    Vector<Scalar, N>& operator/=(const Vector<Scalar, N>& v)
    {
        for (int i = 0; i < N; ++i)
            data_[i] /= v.data_[i];
        return *this;
    }

    //! subtract vector v
    Vector<Scalar, N>& operator-=(const Vector<Scalar, N>& v)
    {
        for (int i = 0; i < N; ++i)
            data_[i] -= v.data_[i];
        return *this;
    }

    //! add vector v
    Vector<Scalar, N>& operator+=(const Vector<Scalar, N>& v)
    {
        for (int i = 0; i < N; ++i)
            data_[i] += v.data_[i];
        return *this;
    }

    //! normalize vector, return normalized vector
    Vector<Scalar, N>& normalize()
    {
        Scalar n = norm(*this);
        if (n > std::numeric_limits<Scalar>::min())
            *this *= 1.0 / n;
        return *this;
    }

    //! return vector with minimum of this and other in each component
    Vector<Scalar, N> minimize(const Vector<Scalar, N>& other)
    {
        for (int i = 0; i < N; ++i)
            if (other[i] < data_[i])
                data_[i] = other[i];
        return *this;
    }

    //! return vector with maximum of this and other in each component
    Vector<Scalar, N> maximize(const Vector<Scalar, N>& other)
    {
        for (int i = 0; i < N; ++i)
            if (other[i] > data_[i])
                data_[i] = other[i];
        return *this;
    }

public:
    /** The N values of type Scalar are the only data members
     of this class. This guarantees 100% compatibility with arrays of type
     Scalar and size N, allowing us to define the cast operators to and from
     arrays and array pointers */
    Scalar data_[N];
};

//== FUNCTIONS ================================================================

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

//! negate vector
template <typename Scalar, int N>
inline Vector<Scalar, N> operator-(const Vector<Scalar, N>& v)
{
    Vector<Scalar, N> vv;
    for (int i = 0; i < N; ++i)
        vv[i]  = -v[i];
    return vv;
}

//! scalar * vector
template <typename Scalar, typename Scalar2, int N>
inline Vector<Scalar, N> operator*(const Scalar2 s, const Vector<Scalar, N>& v)
{
    return Vector<Scalar, N>(v) *= (Scalar)s;
}

//! vector * scalar
template <typename Scalar, typename Scalar2, int N>
inline Vector<Scalar, N> operator*(const Vector<Scalar, N>& v, const Scalar2 s)
{
    return Vector<Scalar, N>(v) *= (Scalar)s;
}

//! vector / scalar
template <typename Scalar, typename Scalar2, int N>
inline Vector<Scalar, N> operator/(const Vector<Scalar, N>& v, const Scalar2 s)
{
    return Vector<Scalar, N>(v) /= Scalar(s);
}

//! component-wise multiplication vector * vector
template <typename Scalar, int N>
inline Vector<Scalar, N> operator*(const Vector<Scalar, N>& v1,
                                   const Vector<Scalar, N>& v2)
{
    return Vector<Scalar, N>(v1) *= v2;
}

//! component-wise division vector / vector
template <typename Scalar, int N>
inline Vector<Scalar, N> operator/(const Vector<Scalar, N>& v1,
                                   const Vector<Scalar, N>& v2)
{
    return Vector<Scalar, N>(v1) /= v2;
}

//! vector + vector
template <typename Scalar, int N>
inline Vector<Scalar, N> operator+(const Vector<Scalar, N>& v0,
                                   const Vector<Scalar, N>& v1)
{
    return Vector<Scalar, N>(v0) += v1;
}

//! vector - vector
template <typename Scalar, int N>
inline Vector<Scalar, N> operator-(const Vector<Scalar, N>& v0,
                                   const Vector<Scalar, N>& v1)
{
    return Vector<Scalar, N>(v0) -= v1;
}

//! compute the Euclidean norm of a vector
template <typename Scalar, int N>
inline Scalar norm(const Vector<Scalar, N>& v)
{
    Scalar s = v[0] * v[0];
    for (int i = 1; i < N; ++i)
        s += v[i] * v[i];
    return (Scalar)sqrt(s);
}

//! compute the Euclidean norm of a vector
template <typename Scalar, int N>
inline Vector<Scalar, N> normalize(const Vector<Scalar, N>& v)
{
    Scalar n = norm(v);
    n        = (n > std::numeric_limits<Scalar>::min()) ? 1.0 / n : 0.0;
    return v * n;
}

//! compute the squared Euclidean norm of a vector
template <typename Scalar, int N>
inline Scalar sqrnorm(const Vector<Scalar, N>& v)
{
    Scalar s = v[0] * v[0];
    for (int i = 1; i < N; ++i)
        s += v[i] * v[i];
    return s;
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


//== TYPEDEFS =================================================================

typedef Vector<float,2>          vec2;
typedef Vector<double,2>        dvec2;
typedef Vector<bool,2>          bvec2;
typedef Vector<int,2>           ivec2;
typedef Vector<unsigned int,2>  uvec2;

typedef Vector<float,3>          vec3;
typedef Vector<double,3>        dvec3;
typedef Vector<bool,3>          bvec3;
typedef Vector<int,3>           ivec3;
typedef Vector<unsigned int,3>  uvec3;

typedef Vector<float,4>          vec4;
typedef Vector<double,4>        dvec4;
typedef Vector<bool,4>          bvec4;
typedef Vector<int,4>           ivec4;
typedef Vector<unsigned int,4>  uvec4;

//=============================================================================
} // namespace surface_mesh
//=============================================================================
