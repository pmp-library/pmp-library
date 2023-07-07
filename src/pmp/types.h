// Copyright 2011-2021 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include <cstdint>

#include "pmp/mat_vec.h"

//! The pmp-library namespace
namespace pmp {

//! \addtogroup core
//! @{

//! Scalar type
#ifdef PMP_SCALAR_TYPE_64
using Scalar = double;
#else
using Scalar = float;
#endif

//! Point type
using Point = Vector<Scalar, 3>;

//! Normal type
using Normal = Vector<Scalar, 3>;

//! Color type
//! \details RGB values in the range of [0,1]
using Color = Vector<Scalar, 3>;

//! Texture coordinate type
using TexCoord = Vector<Scalar, 2>;

// define index type to be used
#ifdef PMP_INDEX_TYPE_64
using IndexType = std::uint_least64_t;
#define PMP_MAX_INDEX UINT_LEAST64_MAX
#else
using IndexType = std::uint_least32_t;
#define PMP_MAX_INDEX UINT_LEAST32_MAX
#endif

//! @}

//! \defgroup core core
//! \brief Core data structure and utilities.

//! \defgroup algorithms algorithms
//! \brief Mesh processing algorithms.

//! \defgroup visualization visualization
//! \brief Visualization tools using OpenGL.

//! \defgroup io io
//! Read and write common mesh formats.

} // namespace pmp
