// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include <map>

#include "pmp/SurfaceMesh.h"

namespace pmp {

//! \brief A class for implicitly fairing a surface mesh.
//! \details See also \cite desbrun_1999_implicit .
//! \ingroup algorithms
class Fairing
{
public:
    //! Construct with mesh to be processed.
    Fairing(SurfaceMesh& mesh);

    // destructor
    ~Fairing();

    //! minimize surface area (class Fairing::fair(1))
    void minimize_area() { fair(1); }

    //! minimize surface curvature (class Fairing::fair(2))
    void minimize_curvature() { fair(2); }

    //! compute surface by solving k-harmonic equation
    //! \throw SolverException in case of failure to solve the linear system
    //! \throw InvalidInputException in case of missing boundary constraints
    void fair(unsigned int k = 2);

private:
    void setup_matrix_row(const Vertex v, VertexProperty<double> vweight,
                          EdgeProperty<double> eweight,
                          unsigned int laplace_degree,
                          std::map<Vertex, double>& row);

    SurfaceMesh& mesh_;

    // property handles
    VertexProperty<Point> points_;
    VertexProperty<bool> vselected_;
    VertexProperty<bool> vlocked_;
    VertexProperty<double> vweight_;
    EdgeProperty<double> eweight_;
    VertexProperty<int> idx_;
};

} // namespace pmp
