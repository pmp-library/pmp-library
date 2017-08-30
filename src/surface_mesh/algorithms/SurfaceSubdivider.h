//=============================================================================
#pragma once
//=============================================================================

#include <surface_mesh/SurfaceMesh.h>

//=============================================================================

namespace surface_mesh {

//=============================================================================

//! \addtogroup algorithms algorithms
//! @{

//! A class providing surface subdivision algorithms
class SurfaceSubdivider
{

public:
    SurfaceSubdivider(SurfaceMesh& mesh);

    //! Perform one step of Loop subdivision
    void catmullClark();

    //! Perform one step of Loop subdivision
    void loop();

    //! Perform one step of sqrt3 subdivision
    void sqrt3();

private:
    SurfaceMesh&                       m_mesh;
    SurfaceMesh::VertexProperty<Point> m_points;
    SurfaceMesh::VertexProperty<bool>  m_vfeature;
    SurfaceMesh::EdgeProperty<bool>    m_efeature;
};

//=============================================================================
//! @}
//=============================================================================
} // namespace surface_mesh
//=============================================================================
