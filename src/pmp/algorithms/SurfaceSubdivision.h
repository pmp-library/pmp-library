// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include "pmp/SurfaceMesh.h"

namespace pmp {

//! \brief A class providing surface subdivision algorithms.
//! \ingroup algorithms
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

} // namespace pmp
