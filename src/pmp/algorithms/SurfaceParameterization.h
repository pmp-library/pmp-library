//=============================================================================
// Copyright (C) 2011-2019 The pmp-library developers
//
// This file is part of the Polygon Mesh Processing Library.
// Distributed under a MIT-style license, see LICENSE.txt for details.
//
// SPDX-License-Identifier: MIT-with-employer-disclaimer
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
    //! \brief Construct with mesh to be parameterized.
    //! \pre The mesh has a boundary.
    //! \throw InvalidInputException if the input precondition is violated.
    SurfaceParameterization(SurfaceMesh& mesh);

    //! \brief Compute discrete harmonic parameterization.
    //! \throw SolverException in case of failure to solve the linear system.
    void harmonic(bool use_uniform_weights = false);

    //! \brief Compute parameterization based on least squares conformal mapping.
    //! \throw SolverException in case of failure to solve the linear system.
    void lscm();

private:
    //! setup boundary constraints: map surface boundary to unit circle
    void setup_boundary_constraints();

    //! setup boundary: pin the two farthest boundary vertices
    void setup_lscm_boundary();

private:
    //! the mesh
    SurfaceMesh& mesh_;
};

//=============================================================================
//! @}
//=============================================================================
} // namespace pmp
//=============================================================================
