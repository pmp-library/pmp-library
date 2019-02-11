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
    void analyze(unsigned int post_smoothing_steps = 0);

    //! compute curvature information for each vertex, optionally followed
    //! by some smoothing iterations of the curvature values
    void analyze_tensor(unsigned int post_smoothing_steps = 0,
                        bool two_ring_neighborhood = false);

    //! return mean curvature
    Scalar mean_curvature(SurfaceMesh::Vertex v) const
    {
        return 0.5 * (min_curvature_[v] + max_curvature_[v]);
    }

    //! return Gaussian curvature
    Scalar gauss_curvature(SurfaceMesh::Vertex v) const
    {
        return min_curvature_[v] * max_curvature_[v];
    }

    //! return minimum (signed) curvature
    Scalar min_curvature(SurfaceMesh::Vertex v) const
    {
        return min_curvature_[v];
    }

    //! return maximum (signed) curvature
    Scalar max_curvature(SurfaceMesh::Vertex v) const
    {
        return max_curvature_[v];
    }

    //! return maximum absolute curvature
    Scalar max_abs_curvature(SurfaceMesh::Vertex v) const
    {
        return std::max(fabs(min_curvature_[v]), fabs(max_curvature_[v]));
    }

    //! convert (precomputed) mean curvature to 1D texture coordinates
    void mean_curvature_to_texture_coordinates() const;

    //! convert (precomputed) Gauss curvature to 1D texture coordinates
    void gauss_curvature_to_texture_coordinates() const;

    //! convert (precomputed) max. abs. curvature to 1D texture coordinates
    void max_curvature_to_texture_coordinates() const;

private:
    //! smooth curvature values
    void smooth_curvatures(unsigned int iterations);

    //! convert curvature values ("v:curv") to 1D texture coordinates
    void curvature_to_texture_coordinates() const;

private:
    SurfaceMesh& mesh_;
    SurfaceMesh::VertexProperty<Scalar> min_curvature_;
    SurfaceMesh::VertexProperty<Scalar> max_curvature_;
};

//=============================================================================
//! @}
//=============================================================================
} // namespace pmp
//=============================================================================
