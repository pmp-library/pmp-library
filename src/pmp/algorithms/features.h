// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include "pmp/surface_mesh.h"

namespace pmp {

//! \brief Mark edges with dihedral angle larger than \p angle as feature.
//! \return The number of feature edges detected.
//! \ingroup algorithms
size_t detect_features(SurfaceMesh& mesh, Scalar angle);

//! \brief Mark all boundary edges as features.
//! \return The number of boundary edges detected.
//! \ingroup algorithms
size_t detect_boundary(SurfaceMesh& mesh);

//! \brief Clear feature and boundary edges.
//! \details Sets all \c "e:feature" and \c "v:feature" properties to \c false.
//! \note This does not remove the corresponding property arrays.
//! \ingroup algorithms
void clear_features(SurfaceMesh& mesh);

} // namespace pmp
