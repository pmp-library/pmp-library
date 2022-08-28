// Copyright 2011-2021 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include "pmp/SurfaceMesh.h"

namespace pmp {

//! \brief A class providing deformation transfer.
//! \ingroup algorithms
class DeformationTransfer
{
public:
    //! Construct with the undeformed versions of source and target mesh.
    //! \pre Requires two triangle meshes with identical connectivity as input.
    //! \throw InvalidInputException in case the input violates the precondition.
    DeformationTransfer(const SurfaceMesh& source, const SurfaceMesh& target);

    //! \brief Transfer the source deformation to the target mesh.
    //! \details See \cite sumner_2004_deftrans and \cite botsch_2006_deftrans for details.
    //! \pre The two input meshes must have the same triangulation as the ones specified in the constructor.
    //! \throw InvalidInputException in case the input violates the precondition.
    void transfer(const SurfaceMesh& deformed_source,
                  SurfaceMesh& deformed_target);

private:
    void compute_cotan_weights(const SurfaceMesh& mesh,
                               EdgeProperty<double>& cotan_weights);
    void lock_vertices(const SurfaceMesh& source_before,
                       const SurfaceMesh& source_after,
                       VertexProperty<bool>& locked);
    void extract_deformation(const SurfaceMesh& before,
                             const SurfaceMesh& after,
                             FaceProperty<dmat3>& defgrad);
    void compute_target_laplacians(const FaceProperty<dmat3>& defgrad,
                                   VertexProperty<dvec3>& laplacians);

private:
    const SurfaceMesh &source_, target_;
};

} // namespace pmp
