// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include "pmp/SurfaceMesh.h"

namespace pmp {

//! \brief A class for Laplacian smoothing
//! \details See also \cite desbrun_1999_implicit and \cite kazhdan_2012
//! \ingroup algorithms
class Smoothing
{
public:
    //! Construct with mesh to be smoothed.
    explicit Smoothing(SurfaceMesh& mesh);

    // Destructor removing any properties allocated in the mesh.
    ~Smoothing();

    //! Perform \p iters iterations of explicit Laplacian smoothing.
    //! Decide whether to use uniform Laplacian or cotan Laplacian (default: cotan).
    void explicit_smoothing(unsigned int iters = 10,
                            bool use_uniform_laplace = false);

    //! \brief Perform implicit Laplacian smoothing.
    //! \param timestep The time step taken.
    //! \param use_uniform_laplace Decide whether to use uniform Laplacian or cotan Laplacian. Default: cotan.
    //! \param rescale Decide whether to re-center and re-scale model after smoothing. Default: true.
    //! \throw SolverException in case of a failure to solve the linear system.
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
    // Initialize cotan/uniform Laplace weights.
    void compute_edge_weights(bool use_uniform_laplace);

    // Initialize cotan/uniform Laplace weights.
    void compute_vertex_weights(bool use_uniform_laplace);

    SurfaceMesh& mesh_;

    // Remember for how many vertices/edges we computed weights.
    // Recompute if numbers change, i.e., the mesh has changed.
    size_t n_edge_weights_{0};
    size_t n_vertex_weights_{0};
};

} // namespace pmp
