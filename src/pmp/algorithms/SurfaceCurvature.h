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
#include <pmp/algorithms/DifferentialGeometry.h>

//=============================================================================

namespace pmp {

//=============================================================================

//! \addtogroup algorithms algorithms
//! @{

//=============================================================================

//! \brief Compute per-vertex curvature (min,max,mean,Gaussian).

//! \details Curvature values for boundary vertices are interpolated from their
//! interior neighbors. Curvature values can be smoothed. See
//! \cite meyer_2003_discrete and \cite cohen-steiner_2003_restricted for
//! details.
class SurfaceCurvature
{
public:
    //! construct with mesh to be analyzed
    SurfaceCurvature(SurfaceMesh& mesh);

    // destructor
    ~SurfaceCurvature();

    //! compute curvature information for each vertex, optionally followed
    //! by some smoothing iterations of the curvature values
    void analyze(unsigned int postSmoothingSteps = 0);

    //! compute curvature information for each vertex, optionally followed
    //! by some smoothing iterations of the curvature values
    void analyzeTensor(unsigned int postSmoothingSteps  = 0,
                       bool         twoRingNeighborhood = false);

    //! return mean curvature
    Scalar meanCurvature(SurfaceMesh::Vertex v) const
    {
        return 0.5 * (m_minCurvature[v] + m_maxCurvature[v]);
    }

    //! return Gaussian curvature
    Scalar gaussCurvature(SurfaceMesh::Vertex v) const
    {
        return m_minCurvature[v] * m_maxCurvature[v];
    }

    //! return minimum (signed) curvature
    Scalar minCurvature(SurfaceMesh::Vertex v) const
    {
        return m_minCurvature[v];
    }

    //! return maximum (signed) curvature
    Scalar maxCurvature(SurfaceMesh::Vertex v) const
    {
        return m_maxCurvature[v];
    }

    //! return maximum absolute curvature
    Scalar maxAbsCurvature(SurfaceMesh::Vertex v) const
    {
        return std::max(fabs(m_minCurvature[v]), fabs(m_maxCurvature[v]));
    }

    //! convert (precomputed) mean curvature to 1D texture coordinates
    void meanCurvatureToTextureCoordinates() const;

    //! convert (precomputed) Gauss curvature to 1D texture coordinates
    void gaussCurvatureToTextureCoordinates() const;

    //! convert (precomputed) max. abs. curvature to 1D texture coordinates
    void maxCurvatureToTextureCoordinates() const;

private:
    //! smooth curvature values
    void smoothCurvatures(unsigned int iterations);

    //! convert curvature values ("v:curv") to 1D texture coordinates
    void curvatureToTextureCoordinates() const;

private:
    SurfaceMesh&                        m_mesh;
    SurfaceMesh::VertexProperty<Scalar> m_minCurvature;
    SurfaceMesh::VertexProperty<Scalar> m_maxCurvature;
};

//=============================================================================
//! @}
//=============================================================================
} // namespace pmp
//=============================================================================
