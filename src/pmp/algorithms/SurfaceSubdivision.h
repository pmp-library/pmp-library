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

    //! \brief Perform one step of Catmull-Clark subdivision.
    //! \details See \cite catmull_1978_recursively for details.
    void catmull_clark();

    //! \brief Perform one step of Loop subdivision.
    //! \details See \cite loop_1987_smooth for details.
    //! \pre Requires a pure triangle mesh as input.
    //! \throw InvalidInputException in case the input violates the precondition.
    void loop();

    //! Perform one step of sqrt3 subdivision.
    //! See \cite kobbelt_2000_sqrt for details.
    //! \pre Requires a pure triangle mesh as input.
    //! \throw InvalidInputException in case the input violates the precondition.
    void sqrt3();

private:
    SurfaceMesh& mesh_;
    VertexProperty<Point> points_;
    VertexProperty<bool> vfeature_;
    EdgeProperty<bool> efeature_;
};

} // namespace pmp
