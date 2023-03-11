// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include "pmp/SurfaceMesh.h"

namespace pmp {

//! \brief Perform explicit Laplacian smoothing.
//! \details See \cite desbrun_1999_implicit for details
//! \param iters The number of iterations performed.
//! \param use_uniform_laplace Decide whether to use uniform Laplacian or cotan Laplacian. Default: cotan.
void explicit_smoothing(SurfaceMesh& mesh, unsigned int iters = 10,
                        bool use_uniform_laplace = false);

//! \brief Perform implicit Laplacian smoothing.
//! \details See \cite desbrun_1999_implicit and \cite kazhdan_2012
//! \param timestep The time step taken.
//! \param use_uniform_laplace Decide whether to use uniform Laplacian or cotan Laplacian. Default: cotan.
//! \param rescale Decide whether to re-center and re-scale model after smoothing. Default: true.
//! \throw SolverException in case of a failure to solve the linear system.
void implicit_smoothing(SurfaceMesh& mesh, Scalar timestep = 0.001,
                        bool use_uniform_laplace = false, bool rescale = true);

} // namespace pmp
