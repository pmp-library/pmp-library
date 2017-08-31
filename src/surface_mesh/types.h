//=============================================================================
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

#include <surface_mesh/Vector.h>
#include <cstdint> // for std::uint_least32_t

//=============================================================================

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

// simple pretty-printing debug macro
#define SM_SHOW(x) std::cerr << #x " = '" << x << "'" << std::endl;

//=============================================================================

//! surface_mesh library namespace
namespace surface_mesh {

//=============================================================================

//! Scalar type
typedef float Scalar;

//! Point type
typedef Vector<Scalar, 3> Point;

//! 3D vector type
//typedef Vector<Scalar, 3> Vec3;

//! Normal type
typedef Vector<Scalar, 3> Normal;

//! Color type
typedef Vector<Scalar, 3> Color;

//! Texture coordinate type
typedef Vector<Scalar, 3> TextureCoordinate;


// define index type to be used
#ifdef SM_INDEX_TYPE_64
typedef std::uint_least64_t IndexType;
#define SM_MAX_INDEX UINT_LEAST64_MAX
#else
typedef std::uint_least32_t IndexType;
#define SM_MAX_INDEX UINT_LEAST32_MAX
#endif


//=============================================================================
} // namespace surface_mesh
//=============================================================================
