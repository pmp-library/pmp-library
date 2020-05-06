// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include <map>

#include "pmp/SurfaceMesh.h"

namespace pmp {

//! \brief A class for implicitly fairing a surface mesh.
//! \details See also \cite desbrun_1999_implicit .
//! \ingroup algorithms
class SurfaceFairing
{
public:
    //! Construct with mesh to be processed.
    SurfaceFairing(SurfaceMesh& mesh);

    // destructor
    ~SurfaceFairing();

    //! minimize surface area (class SurfaceFairing::fair(1))
    void minimize_area() { fair(1); }

    //! minimize surface curvature (class SurfaceFairing::fair(2))
    void minimize_curvature() { fair(2); }

    //! compute surface by solving k-harmonic equation
    void fair(unsigned int k = 2);

private:
    void setup_matrix_row(const Vertex v, VertexProperty<double> vweight,
                          EdgeProperty<double> eweight,
                          unsigned int laplace_degree,
                          std::map<Vertex, double>& row);

private:
    SurfaceMesh& mesh_; //!< the mesh

    // property handles
    VertexProperty<Point> points_;
    VertexProperty<bool> vselected_;
    VertexProperty<bool> vlocked_;
    VertexProperty<double> vweight_;
    EdgeProperty<double> eweight_;
    VertexProperty<int> idx_;
};

} // namespace pmp
