// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include <map>

#include "pmp/SurfaceMesh.h"

namespace pmp {

//! minimize surface area
//! \sa fair()
//! \ingroup algorithms
void minimize_area(SurfaceMesh& mesh);

//! minimize surface curvature
//! \sa fair()
//! \ingroup algorithms
void minimize_curvature(SurfaceMesh& mesh);

//! \brief Implicit surface fairing.
//! \details Computes a surface by solving k-harmonic equation. See also \cite desbrun_1999_implicit .
//! \throw SolverException in case of failure to solve the linear system
//! \throw InvalidInputException in case of missing boundary constraints
//! \ingroup algorithms
void fair(SurfaceMesh& mesh, unsigned int k = 2);

} // namespace pmp
