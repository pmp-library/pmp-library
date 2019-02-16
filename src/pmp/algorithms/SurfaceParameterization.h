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

//! \brief A class for surface parameterization.
//! \details See \cite levy_2002_least and \cite desbrun_2002_intrinsic
//! for more details.
class SurfaceParameterization
{
public:
    //! give a mesh in the constructor
    SurfaceParameterization(SurfaceMesh& mesh);

    //! compute discrete harmonic parameterization
    void harmonic(bool use_uniform_weights = false);

    //! compute parameterization based on least squares conformal mapping
    void lscm();

private:
    //! setup boundary constraints: map surface boundary to unit circle
    bool setup_boundary_constraints();

    //! setup boundary: pin the two farthest boundary vertices
    bool setup_lscm_boundary();

private:
    //! the mesh
    SurfaceMesh& mesh_;
};

//=============================================================================
//! @}
//=============================================================================
} // namespace pmp
//=============================================================================
