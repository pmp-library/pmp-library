// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include "pmp/SurfaceMesh.h"

namespace pmp {

//! \brief Compute per-vertex curvature (min,max,mean,Gaussian).
//! \details Curvature values for boundary vertices are interpolated from their
//! interior neighbors. Curvature values can be smoothed. See
//! \cite meyer_2003_discrete and \cite cohen-steiner_2003_restricted for
//! details.
//! \ingroup algorithms
class SurfaceCurvature
{
public:
    //! construct with mesh to be analyzed
    SurfaceCurvature(SurfaceMesh& mesh);

    //! destructor
    ~SurfaceCurvature();

    //! compute curvature information for each vertex, optionally followed
    //! by some smoothing iterations of the curvature values
    void analyze(unsigned int post_smoothing_steps = 0);

    //! compute curvature information for each vertex, optionally followed
    //! by some smoothing iterations of the curvature values
    void analyze_tensor(unsigned int post_smoothing_steps = 0,
                        bool two_ring_neighborhood = false);

    //! return mean curvature
    Scalar mean_curvature(Vertex v) const
    {
        return 0.5 * (min_curvature_[v] + max_curvature_[v]);
    }

    //! return Gaussian curvature
    Scalar gauss_curvature(Vertex v) const
    {
        return min_curvature_[v] * max_curvature_[v];
    }

    //! return minimum (signed) curvature
    Scalar min_curvature(Vertex v) const { return min_curvature_[v]; }

    //! return maximum (signed) curvature
    Scalar max_curvature(Vertex v) const { return max_curvature_[v]; }

    //! return maximum absolute curvature
    Scalar max_abs_curvature(Vertex v) const
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
    VertexProperty<Scalar> min_curvature_;
    VertexProperty<Scalar> max_curvature_;
};

} // namespace pmp
