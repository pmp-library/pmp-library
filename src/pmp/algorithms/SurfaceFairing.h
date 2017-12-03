//=============================================================================
// Copyright (C) 2011-2017 The pmp-library developers
// All rights reserved.
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

//! \brief A class for fairing a surface mesh.
//! \details The class provides two main smoothing functions:
//!   \li \c implicitSmooth() to perform iterative bi-Laplacian smoothing
//!   \li \c fair() to compute a limit surface.
class SurfaceFairing
{
public:
    //! constructor
    SurfaceFairing(SurfaceMesh& mesh);

    //! destructor
    ~SurfaceFairing();

    //! perform \c n iterations of implicit bi-Laplacian smoothing with
    //! a timestep \c t.
    void implicitSmooth(unsigned int n, Scalar t);

    //! compute limit surface (minimize curvature energy)
    void fair();

private:
    void setupMatrixRow(const SurfaceMesh::Vertex           v,
                        SurfaceMesh::VertexProperty<double> vweight,
                        SurfaceMesh::EdgeProperty<double>   eweight,
                        unsigned int                        laplaceDegree,
                        std::map<SurfaceMesh::Vertex, double>& row);

private:
    SurfaceMesh& m_mesh; //!< the mesh

    // property handles
    SurfaceMesh::VertexProperty<Point>  m_points;
    SurfaceMesh::VertexProperty<bool>   m_vselected;
    SurfaceMesh::VertexProperty<bool>   m_vlocked;
    SurfaceMesh::VertexProperty<double> m_vweight;
    SurfaceMesh::EdgeProperty<double>   m_eweight;
    SurfaceMesh::VertexProperty<int>    m_idx;
};

//=============================================================================
//! @}
//=============================================================================
} // namespace pmp
//=============================================================================
