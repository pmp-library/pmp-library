// Copyright 2011-2021 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include "pmp/SurfaceMesh.h"

namespace pmp {

//! \addtogroup algorithms
//! @{

//! Factory class to generate different types of basic shapes.
class SurfaceFactory
{
public:
    //! Generate tetrahedron.
    static SurfaceMesh tetrahedron();

    //! Generate hexahedron.
    static SurfaceMesh hexahedron();

    //! Generate octahedron.
    static SurfaceMesh octahedron();

    //! Generate dodecahedron.
    static SurfaceMesh dodecahedron();

    //! Generate icosahedron.
    static SurfaceMesh icosahedron();

    //! \brief Generate icosphere refined by \p n_subdivisions .
    //! \details Uses Loop subdivision to refine the initial icosahedron.
    //! \sa SurfaceSubdivision
    static SurfaceMesh icosphere(size_t n_subdivisions = 3);

    //! \brief Generate quad sphere refined by \p n_subdivisions .
    //! \details Uses Catmull-Clark subdivision to refine the initial hexahedron.
    //! \sa SurfaceSubdivision
    static SurfaceMesh quad_sphere(size_t n_subdivisions = 3);

    //! Generate UV sphere with given \p center, \p radius, \p n_slices, and \p n_stacks.
    static SurfaceMesh uv_sphere(const Point& center = Point(0, 0, 0),
                                 Scalar radius = 1.0, size_t n_slices = 15,
                                 size_t n_stacks = 15);
};

//! @}

} // namespace pmp
