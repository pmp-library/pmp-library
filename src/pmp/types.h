//=============================================================================
// Copyright (C) 2011-2017 The pmp-library developers
//
// This file is part of the Polygon Mesh Processing Library.
// Distributed under the terms of the MIT license, see LICENSE.txt for details.
//
// SPDX-License-Identifier: MIT
//=============================================================================
#pragma once
//=============================================================================

#include <pmp/MatVec.h>
#include <cstdint> // for std::uint_least32_t

//=============================================================================

//! \def PMP_ASSERT(x)
//! Custom assert macro that allows to silence unused variable warnings with no
//! overhead. Generates no code in release mode since if the argument to
//! sizeof() is an expression it is not evaluated. In debug mode we just fall
//! back to the default assert().
#ifdef NDEBUG
#define PMP_ASSERT(x)    \
    do                   \
    {                    \
        (void)sizeof(x); \
    } while (0)
#else
#define PMP_ASSERT(x) assert(x)
#endif

//! \def PMP_SHOW(x)
//! A simple pretty-printing debug macro. Prints the expression and its value.
#define PMP_SHOW(x) std::cerr << #x " = '" << x << "'" << std::endl;

//=============================================================================

//! The pmp-library namespace
namespace pmp {

//=============================================================================

//! Scalar type
typedef float Scalar;

//! Point type
typedef Vector<Scalar, 3> Point;

//! Normal type
typedef Vector<Scalar, 3> Normal;

//! Color type
typedef Vector<Scalar, 3> Color;

//! Texture coordinate type
typedef Vector<Scalar, 2> TextureCoordinate, TexCoord;

// define index type to be used
#ifdef PMP_INDEX_TYPE_64
typedef std::uint_least64_t IndexType;
#define PMP_MAX_INDEX UINT_LEAST64_MAX
#else
typedef std::uint_least32_t IndexType;
#define PMP_MAX_INDEX UINT_LEAST32_MAX
#endif

//! \defgroup algorithms algorithms
//! \brief Geometry processing algorithms.

//! \defgroup geometry geometry
//! \brief Data structures for representing geometric data sets.

//! \defgroup gl gl
//! \brief Visualization tools using OpenGL.

//! \defgroup io io
//! \brief Classes for reading and writing geometry data structures.

//=============================================================================
} // namespace pmp
//=============================================================================
