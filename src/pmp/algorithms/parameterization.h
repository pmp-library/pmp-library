// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include "pmp/surface_mesh.h"

namespace pmp {

//! \brief Compute discrete harmonic parameterization.
//! \details See \cite desbrun_2002_intrinsic for details.
//! \note This algorithm works on general polygon meshes.
//! \pre The mesh has a boundary.
//! \throw InvalidInputException if the input precondition is violated.
//! \throw SolverException in case of failure to solve the linear system.
//! \note This algorithm works on general polygon meshes.
//! \ingroup algorithms
void harmonic_parameterization(SurfaceMesh& mesh,
                               bool use_uniform_weights = false);

//! \brief Compute parameterization based on least squares conformal mapping.
//! \details See \cite levy_2002_least for details.
//! \pre The mesh has a boundary.
//! \pre Input mesh needs to be a triangle mesh.
//! \throw InvalidInputException if the input precondition is violated.
//! \throw SolverException in case of failure to solve the linear system.
//! \ingroup algorithms
void lscm_parameterization(SurfaceMesh& mesh);

} // namespace pmp
