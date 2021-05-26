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

    //! \brief Generate a plane mesh.
    //! \details Generates a pure quad mesh in the x-y plane with origin (0,0,0)
    //! and side length 1.
    //! \param resolution Number of faces in each direction. Needs to be >= 1. Default: 4.
    static SurfaceMesh plane(size_t resolution = 4);

    //! \brief Generate a cone mesh.
    //! \details Generates a polygonal mesh of a cone. The circular base lies in
    //! the x-y-plane and the tip points in positive z-direction.
    //! \param n_subdivisions Number of subdivisions of the base circle. Needs to be >= 3. Default: 30.
    //! \param radius Radius of the base circle. Default: 1.
    //! \param height Height of the the cone. Default: 2.5.
    static SurfaceMesh cone(size_t n_subdivisions = 30, Scalar radius = 1.0,
                            Scalar height = 2.5);

    //! \brief Generate a cylinder mesh.
    //! \details Generates a polygonal mesh of a cylinder. The cylinder is
    //! oriented in z-direction.
    //! \param n_subdivisions Number of subdivisions of the cylinder. Needs to be >= 3. Default: 30.
    //! \param radius Radius of the cylinder. Default: 1.
    //! \param height Height of the cylinder. Default: 2.5.
    static SurfaceMesh cylinder(size_t n_subdivisions = 30, Scalar radius = 1.0,
                                Scalar height = 2.5);

    //! \brief Generate a torus mesh.
    //! \details Generates a quad mesh of a torus with its major circle in the x-y plane.
    //! \param radial_resolution Number of subdivisions of the major circle. Needs to be >= 3. Default: 20.
    //! \param tubular_resolution Number of subdivisions of along the tube. Needs to be >= 3. Default: 40.
    //! \param radius Radius of the major circle. Default: 1.
    //! \param thickness Thickness of the tube. Default: 0.4.
    static SurfaceMesh torus(size_t radial_resolution = 20,
                             size_t tubular_resolution = 40,
                             Scalar radius = 1.0, Scalar thickness = 0.4);
};

//! @}

} // namespace pmp
