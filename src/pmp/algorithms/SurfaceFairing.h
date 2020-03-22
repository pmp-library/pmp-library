//=============================================================================
// Copyright (C) 2011-2019 The pmp-library developers
//
// This file is part of the Polygon Mesh Processing Library.
// Distributed under a MIT-style license, see LICENSE.txt for details.
//
// SPDX-License-Identifier: MIT-with-employer-disclaimer
//=============================================================================
#pragma once
//=============================================================================

#include <pmp/SurfaceMesh.h>
#include <map>

//=============================================================================

namespace pmp {

//=============================================================================

//! \addtogroup algorithms algorithms
//! @{

//=============================================================================

//! \brief A class for implicitly fairing a surface mesh.
//! \details See also \cite desbrun_1999_implicit .
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
    //! \throw SolverException in case of failure to solve the linear system
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

//=============================================================================
//! @}
//=============================================================================
} // namespace pmp
//=============================================================================
