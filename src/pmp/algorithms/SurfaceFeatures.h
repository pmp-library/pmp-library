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

//! Detect and mark feature edges based on boundary or dihedral angle
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

//=============================================================================
//! @}
//=============================================================================
} // namespace pmp
//=============================================================================
