// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include "pmp/SurfaceMesh.h"

namespace pmp {

//! \brief Detect and mark feature edges based on boundary or dihedral angle.
//! \ingroup algorithms
class Features
{
public:
    //! \brief Construct with mesh to be analyzed.
    //! \details Adds two \c bool properties to the mesh if they are not already
    //! existing:
    //!  - \c "e:feature" to mark feature edges
    //!  - \c "v:feature" to mark feature vertices.
    //!
    //! \note The class never removes the marker properties so that other
    //! algorithms can re-use this information. Cleaning up the properties when
    //! they are no longer needed is the responsibility of the caller.
    Features(SurfaceMesh& mesh);

    //! \brief Clear feature and boundary edges.
    //! \details Sets all \c "e:feature" and \c "v:feature" properties to \c false.
    //! \note This does not remove the corresponding property arrays.
    void clear();

    //! \brief Mark all boundary edges as features.
    //! \return The number of boudary edges detected.
    size_t detect_boundary();

    //! \brief Mark edges with dihedral angle larger than \p angle as feature.
    //! \return The number of feature edges detected.
    size_t detect_angle(Scalar angle);

private:
    SurfaceMesh& mesh_;
    VertexProperty<bool> vfeature_;
    EdgeProperty<bool> efeature_;
};

} // namespace pmp
