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

//! A class providing surface subdivision algorithms
class SurfaceSubdivision
{

public:
    //! Construct with mesh to be subdivided.
    SurfaceSubdivision(SurfaceMesh& mesh);

    //! Perform one step of Catmull-Clark subdivision.
    //! See \cite catmull_1978_recursively for details.
    void catmull_clark();

    //! Perform one step of Loop subdivision.
    //! See \cite loop_1987_smooth for details.
    void loop();

    //! Perform one step of sqrt3 subdivision.
    //! See \cite kobbelt_2000_sqrt for details.
    void sqrt3();

private:
    SurfaceMesh& mesh_;
    VertexProperty<Point> points_;
    VertexProperty<bool> vfeature_;
    EdgeProperty<bool> efeature_;
};

//=============================================================================
//! @}
//=============================================================================
} // namespace pmp
//=============================================================================
