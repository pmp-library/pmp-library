// Copyright 2011-2022 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include "pmp/SurfaceMesh.h"

namespace pmp {

//! \brief A class providing surface subdivision algorithms.
//! \ingroup algorithms
class Subdivision
{
public:
    //! Construct with mesh to be subdivided.
    Subdivision(SurfaceMesh& mesh);

    //! \brief Perform one step of Catmull-Clark subdivision.
    //! \details See \cite catmull_1978_recursively for details.
    void catmull_clark();

    //! \brief Perform one step of Loop subdivision.
    //! \details See \cite loop_1987_smooth for details.
    //! \pre Requires a pure triangle mesh as input.
    //! \throw InvalidInputException in case the input violates the precondition.
    void loop();

    //! \brief Perform one step of quad-tri subdivision.
    //! \details See \cite stam_2003_subdiv for details.
    void quad_tri();

private:
    SurfaceMesh& mesh_;
    VertexProperty<Point> points_;
    VertexProperty<bool> vfeature_;
    EdgeProperty<bool> efeature_;
};

} // namespace pmp
