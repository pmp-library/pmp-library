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

//! \addtogroup core core
//! @{

//! Scalar type
typedef float Scalar;

//! Point type
typedef Vector<Scalar, 3> Point;

//! Normal type
typedef Vector<Scalar, 3> Normal;

//! Color type
typedef Vector<Scalar, 3> Color;

//! Texture coordinate type
typedef Vector<Scalar, 2> TexCoord;

// define index type to be used
#ifdef PMP_INDEX_TYPE_64
typedef std::uint_least64_t IndexType;
#define PMP_MAX_INDEX UINT_LEAST64_MAX
#else
typedef std::uint_least32_t IndexType;
#define PMP_MAX_INDEX UINT_LEAST32_MAX
#endif

//! Common IO flags for reading and writing
struct IOFlags
{
    IOFlags() {}
    bool use_binary = false;         //!< read / write binary format
    bool use_vertex_normals = false; //!< read / write vertex normals
    bool use_vertex_colors = false;  //!< read / write vertex colors
    bool use_vertex_texcoords =
        false;                     //!< read / write vertex texture coordinates
    bool use_face_normals = false; //!< read / write face normals
    bool use_face_colors = false;  //!< read / write face colors
    bool use_halfedge_texcoords =
        false; //!< read / write halfedge texture coordinates
};

//! @}

//! \defgroup algorithms algorithms
//! \brief Geometry processing algorithms.

//! \defgroup core core
//! \brief Core data structure and utilities.

//! \defgroup visualization visualization
//! \brief Visualization tools using OpenGL.

//=============================================================================
} // namespace pmp
//=============================================================================
