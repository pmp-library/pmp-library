// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include "pmp/surface_mesh.h"

namespace pmp {

//! Type of curvature to be computed
//! \ingroup algorithms
enum class Curvature
{
    min,    //!< minimum curvature
    max,    //!< maximum curvature
    mean,   //!< mean curvature
    gauss,  //!< Gauss curvature
    max_abs //!< maximum absolute curvature
};

//! \brief Compute per-vertex curvature (min,max,mean,Gaussian).
//! \details Curvature values for boundary vertices are interpolated from their
//! interior neighbors. Curvature values can be smoothed. See
//! \cite meyer_2003_discrete and \cite cohen-steiner_2003_restricted for
//! details.
//! \note This algorithm works on general polygon meshes.
//! \ingroup algorithms
void curvature(SurfaceMesh& mesh, Curvature c = Curvature::mean,
               int smoothing_steps = 0, bool use_tensor = false,
               bool use_two_ring = false);

//! convert curvature values "v:curv" to 1D texture coordinates stored in vertex property "v:tex"
void curvature_to_texture_coordinates(SurfaceMesh& mesh);

} // namespace pmp
