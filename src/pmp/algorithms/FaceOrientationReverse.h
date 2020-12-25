// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include "pmp/SurfaceMesh.h"

namespace pmp {

//! \brief A class for reversing face orientation
//! \ingroup algorithms
class FaceOrientationReverse
{
public:
    //! Construct with mesh to be reversing face orientation.
    FaceOrientationReverse(SurfaceMesh& mesh);

    //! \brief Reverse the orientation of faces. 
    //! \note If mesh has normals, the direction of face normals should be updated.
    void reverse();

private:
    //! \brief Reverse halfedge
    void reverse_halfedge(Halfedge halfedge);

private:
    //! the mesh
    SurfaceMesh& mesh_;
};

} // namespace pmp
