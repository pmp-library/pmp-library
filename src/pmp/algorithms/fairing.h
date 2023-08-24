// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include "pmp/surface_mesh.h"

namespace pmp {

//! Minimize surface area.
//! \note This algorithm works on general polygon meshes.
//! \sa fair()
//! \ingroup algorithms
void minimize_area(SurfaceMesh& mesh);

//! Minimize surface curvature.
//! \note This algorithm works on general polygon meshes.
//! \sa fair()
//! \ingroup algorithms
void minimize_curvature(SurfaceMesh& mesh);

//! \brief Implicit surface fairing.
//! \details Computes a surface by solving k-harmonic equation. See also \cite desbrun_1999_implicit .
//! \note This algorithm works on general polygon meshes.
//! \throw SolverException in case of failure to solve the linear system
//! \throw InvalidInputException in case of missing boundary constraints
//! \ingroup algorithms
void fair(SurfaceMesh& mesh, unsigned int k = 2);

} // namespace pmp
