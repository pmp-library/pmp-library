// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include <pmp/SurfaceMesh.h>

namespace pmp {

//! \addtogroup algorithms
//! @{

//! Generate tetrahedron.
SurfaceMesh tetrahedron();

//! Generate hexahedron.
SurfaceMesh hexahedron();

//! Generate octahedron.
SurfaceMesh octahedron();

//! Generate dodecahedron.
SurfaceMesh dodecahedron();

//! Generate icosahedron.
SurfaceMesh icosahedron();

//! \brief Generate icosphere refined by \p n_subdivisions .
//! \details Uses Loop subdivision to refine the initial icosahedron.
//! \sa SurfaceSubdivision
SurfaceMesh icosphere(size_t n_subdivisions = 3);

//! \brief Generate quad sphere refined by \p n_subdivisions .
//! \details Uses Catmull-Clark subdivision to refine the initial hexahedron.
//! \sa SurfaceSubdivision
SurfaceMesh quad_sphere(size_t n_subdivisions = 3);

//! Generate UV sphere with given \p center, \p radius, \p n_slices, and \p n_stacks.
SurfaceMesh uv_sphere(const Point& center = Point(0, 0, 0), Scalar radius = 1.0,
                      size_t n_slices = 15, size_t n_stacks = 15);

//! @}

} // namespace pmp