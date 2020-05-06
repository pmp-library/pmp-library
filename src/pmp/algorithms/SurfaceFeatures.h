// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include "pmp/SurfaceMesh.h"

namespace pmp {

//! \brief Detect and mark feature edges based on boundary or dihedral angle.
//! \ingroup algorithms
class SurfaceFeatures
{
public:
    //! Construct with mesh to be analyzed.
    SurfaceFeatures(SurfaceMesh& mesh);

    //! Clear features.
    void clear();

    //! Mark all boundary edges as features.
    void detect_boundary();

    //! Mark edges with dihedral angle larger than \p angle as feature.
    void detect_angle(Scalar angle);

private:
    SurfaceMesh& mesh_;

    VertexProperty<bool> vfeature_;
    EdgeProperty<bool> efeature_;
};

} // namespace pmp
