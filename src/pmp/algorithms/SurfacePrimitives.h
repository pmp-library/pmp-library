// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include <pmp/SurfaceMesh.h>

namespace pmp {

//! \brief A class to generate surface primitives
//! \ingroup algorithms
class SurfacePrimitives
{
public:
    SurfacePrimitives(SurfaceMesh& mesh) : mesh_(mesh){};

    //! Generate UV sphere with given \p center, \p radius, \p n_slices, and \p n_stacks.
    void uv_sphere(const Point& center = Point(0, 0, 0), Scalar radius = 1.0,
                   size_t n_slices = 15, size_t n_stacks = 15);

    //! Generate unit cube quad mesh.
    void unit_cube();

    //! Generate icosahedron.
    void icosahedron();

    //! \brief Generate icosphere refined by \p n_subdivisions .
    //! \details Uses Loop subdivision to refine the initial icosahedron.
    //! \sa SurfaceSubdivision
    void icosphere(size_t n_subdivisions = 3);

private:
    SurfaceMesh& mesh_;
};

} // namespace pmp