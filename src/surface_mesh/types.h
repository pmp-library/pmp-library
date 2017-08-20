//=============================================================================
// Copyright (C) 2013 Graphics & Geometry Group, Bielefeld University
// Copyright (C) 2017 Daniel Sieger
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public License
// as published by the Free Software Foundation, version 2.
//
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//=============================================================================
#pragma once
//=============================================================================

#include <surface_mesh/Vector.h>

#ifdef NDEBUG
// this macro allows to avoid unused variable warnings with no overhead
#define SM_ASSERT(x)        \
    do                   \
    {                    \
        (void)sizeof(x); \
    } while (0)
#else
#define SM_ASSERT(x) assert(x)
#endif

//=============================================================================

//! surface_mesh library namespace
namespace surface_mesh {

//=============================================================================

//! Scalar type
typedef float Scalar;

//! Point type
typedef Vector<Scalar, 3> Point;

//! 3D vector type
typedef Vector<Scalar, 3> Vec3;

//! Normal type
typedef Vector<Scalar, 3> Normal;

//! Color type
typedef Vector<Scalar, 3> Color;

//! Texture coordinate type
typedef Vector<Scalar, 3> TextureCoordinate;

//=============================================================================
} // namespace surface_mesh
//=============================================================================
