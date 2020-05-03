// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include "pmp/SurfaceMesh.h"

namespace pmp {

//! \brief A class for Laplacian smoothing
//! \details See also \cite desbrun_1999_implicit and \cite kazhdan_2012
//! \ingroup algorithms
class SurfaceSmoothing
{
public:
    //! Construct with mesh to be smoothed.
    SurfaceSmoothing(SurfaceMesh& mesh);

    // destructor
    ~SurfaceSmoothing();

    //! Perform \p iters iterations of explicit Laplacian smoothing.
    //! Decide whether to use uniform Laplacian or cotan Laplacian (default: cotan).
    void explicit_smoothing(unsigned int iters = 10,
                            bool use_uniform_laplace = false);

    //! Perform implicit Laplacian smoothing with \p timestep.
    //! Decide whether to use uniform Laplacian or cotan Laplacian (default: cotan).
    //! Decide whether to re-center and re-scale model after smoothing (default: true).
    void implicit_smoothing(Scalar timestep = 0.001,
                            bool use_uniform_laplace = false,
                            bool rescale = true);

    //! Initialize edge and vertex weights.
    void initialize(bool use_uniform_laplace = false)
    {
        compute_edge_weights(use_uniform_laplace);
        compute_vertex_weights(use_uniform_laplace);
    }

private:
    //! Initialize cotan/uniform Laplace weights.
    void compute_edge_weights(bool use_uniform_laplace);

    //! Initialize cotan/uniform Laplace weights.
    void compute_vertex_weights(bool use_uniform_laplace);

private:
    //! the mesh
    SurfaceMesh& mesh_;

    // remember for how many vertices/edges we computed weights
    // recompute if numbers change (i.e. mesh has changed)
    unsigned int how_many_edge_weights_;
    unsigned int how_many_vertex_weights_;
};

} // namespace pmp
