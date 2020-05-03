// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include "pmp/SurfaceMesh.h"

namespace pmp {

//! \brief A class for surface parameterization.
//! \details See \cite levy_2002_least and \cite desbrun_2002_intrinsic
//! for more details.
//! \ingroup algorithms
class SurfaceParameterization
{
public:
    //! Construct with mesh to be parameterized.
    SurfaceParameterization(SurfaceMesh& mesh);

    //! Compute discrete harmonic parameterization.
    void harmonic(bool use_uniform_weights = false);

    //! Compute parameterization based on least squares conformal mapping.
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

} // namespace pmp
