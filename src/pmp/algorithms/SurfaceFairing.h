//=============================================================================
// Copyright (C) 2011-2019 The pmp-library developers
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
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

//! \brief A class for implicit fairing a surface mesh.
//! \details See also \cite desbrun_1999_implicit .
class SurfaceFairing
{
public:
    //! constructor
    SurfaceFairing(SurfaceMesh& mesh);

    //! destructor
    ~SurfaceFairing();

    //! minimize surface area (class SurfaceFairing::fair(1))
    void minimize_area() { fair(1); }

    //! minimize surface curvature (class SurfaceFairing::fair(2))
    void minimize_curvature() { fair(2); }

    //! compute surface by solving k-harmonic equation
    void fair(unsigned int k = 2);

private:
    void setup_matrix_row(const Vertex v,
                          VertexProperty<double> vweight,
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
