//=============================================================================

#include "FeatureDetection.h"

//=============================================================================

namespace surface_mesh {

//=============================================================================

FeatureDetection::FeatureDetection(SurfaceMesh& mesh) : m_mesh(mesh)
{
    m_vfeature = m_mesh.vertexProperty("v:feature", false);
    m_efeature = m_mesh.edgeProperty("e:feature", false);
}

//-----------------------------------------------------------------------------

void FeatureDetection::clear()
{
    for (auto v : m_mesh.vertices())
        m_vfeature[v] = false;

    for (auto e : m_mesh.edges())
        m_efeature[e] = false;
}

//-----------------------------------------------------------------------------

void FeatureDetection::detectBoundary()
{
    for (auto v : m_mesh.vertices())
        if (m_mesh.isBoundary(v))
            m_vfeature[v] = true;

    for (auto e : m_mesh.edges())
        if (m_mesh.isBoundary(e))
            m_efeature[e] = true;
}

//-----------------------------------------------------------------------------

void FeatureDetection::detectAngle(Scalar angle)
{
    const Scalar feature_cosine = cos(angle / 180.0 * M_PI);

    for (auto e : m_mesh.edges())
    {
        if (!m_mesh.isBoundary(e))
        {
            const auto f0 = m_mesh.face(m_mesh.halfedge(e, 0));
            const auto f1 = m_mesh.face(m_mesh.halfedge(e, 1));

            const Normal n0 = m_mesh.computeFaceNormal(f0);
            const Normal n1 = m_mesh.computeFaceNormal(f1);

            if (dot(n0, n1) < feature_cosine)
            {
                m_efeature[e] = true;
                m_vfeature[m_mesh.vertex(e, 0)] = true;
                m_vfeature[m_mesh.vertex(e, 1)] = true;
            }
        }
    }
}

//=============================================================================
} // namespace surface_mesh
//=============================================================================
