//=============================================================================
#pragma once
//=============================================================================

#include <surface_mesh/SurfaceMesh.h>

//=============================================================================

namespace surface_mesh {

//=============================================================================

/// \addtogroup algorithms
/// @{

//=============================================================================

//! Detect and mark feature edges based on boundary or dihedral angle
class FeatureDetection
{
public:
    //! constructor
    FeatureDetection(SurfaceMesh& mesh);

    //! clear features
    void clear();

    //! Mark all boundary edges as features
    void detectBoundary();

    //! Mark edges with large dihedral angle as feature
    void detectAngle(Scalar angle);

private:
    SurfaceMesh& m_mesh;

    SurfaceMesh::VertexProperty<bool> m_vfeature;
    SurfaceMesh::EdgeProperty<bool>   m_efeature;
};

//=============================================================================
//! @}
//=============================================================================
} // namespace surface_mesh
//=============================================================================
