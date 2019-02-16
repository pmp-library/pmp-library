//=============================================================================
// Copyright (C) 2011-2019 The pmp-library developers
//
// This file is part of the Polygon Mesh Processing Library.
// Distributed under the terms of the MIT license, see LICENSE.txt for details.
//
// SPDX-License-Identifier: MIT
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
    //! constructor
    SurfaceFeatures(SurfaceMesh& mesh);

    //! clear features
    void clear();

    //! Mark all boundary edges as features
    void detect_boundary();

    //! Mark edges with large dihedral angle as feature
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
