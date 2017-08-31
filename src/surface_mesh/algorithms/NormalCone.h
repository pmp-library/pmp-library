//=============================================================================
#pragma once
//=============================================================================

#include <surface_mesh/types.h>

#include <cmath>

//=============================================================================

namespace surface_mesh {

//=============================================================================

class NormalCone
{
public:
    //! default constructor (not initialized)
    NormalCone() {}

    //! Initialize cone with center (unit vector) and angle (radius in radians)
    NormalCone(const Normal& normal, Scalar angle = 0.0)
        : m_centerNormal(normal), m_angle(angle)
    {
    }

    //! returns center normal
    const Normal& centerNormal() const { return m_centerNormal; }

    //! returns size of cone (radius in radians)
    Scalar angle() const { return m_angle; }

    //! merge *this with n.
    NormalCone& merge(const Normal& n) { return merge(NormalCone(n)); }

    //! merge *this with nc. *this will then enclose both cones.
    NormalCone& merge(const NormalCone& nc)
    {
        const Scalar dp = dot(m_centerNormal, nc.m_centerNormal);

        // axes point in same direction
        if (dp > 0.99999)
        {
            m_angle = std::max(m_angle, nc.m_angle);
        }

        // axes point in opposite directions
        else if (dp < -0.99999)
        {
            m_angle = 2 * M_PI;
        }

        else
        {
            // new angle
            Scalar centerAngle = acos(dp);
            Scalar minAngle    = std::min(-m_angle, centerAngle - nc.m_angle);
            Scalar maxAngle    = std::max(m_angle, centerAngle + nc.m_angle);
            m_angle            = 0.5 * (maxAngle - minAngle);

            // axis by SLERP
            Scalar axisAngle = 0.5 * (minAngle + maxAngle);
            m_centerNormal   = ((m_centerNormal * sin(centerAngle - axisAngle) +
                               nc.m_centerNormal * sin(axisAngle)) /
                              sin(centerAngle));
        }

        return *this;
    }

private:
    Normal m_centerNormal;
    Scalar m_angle;
};

//=============================================================================
} // namespace surface_mesh
//=============================================================================
