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

#include <pmp/SurfaceMesh.h>

//=============================================================================

namespace pmp {

//=============================================================================

//! \addtogroup algorithms algorithms
//! @{

//=============================================================================

//! \brief A class for Laplacian smoothing
//! \details See also \cite desbrun_1999_implicit .
class SurfaceSmoothing
{
public:
    //! give a mesh in the constructor
    SurfaceSmoothing(SurfaceMesh& mesh) : mesh_(mesh){};

    //! Perform \c iters iterations of explicit Laplacian smoothing.
    void explicit_smoothing(unsigned int iters = 10,
                            bool use_uniform_laplace = false);

    //! Perform implicit Laplacian smoothing with \c timestep.
    void implicit_smoothing(Scalar timestep = 0.001,
                            bool use_uniform_laplace = false);

private:
    //! the mesh
    SurfaceMesh& mesh_;
};

//=============================================================================
//! @}
//=============================================================================
} // namespace pmp
//=============================================================================
