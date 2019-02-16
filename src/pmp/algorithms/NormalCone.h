//=============================================================================
// Copyright (C) 2011-2019 The pmp-library developers
//
// This file is part of the Polygon Mesh Processing Library.
// Distributed under the terms of the MIT license, see LICENSE.txt for details.
//
// SPDX-License-Identifier: MIT
//=============================================================================
#pragma once
//=============================================================================

#include <pmp/Types.h>

#include <cmath>

//=============================================================================

namespace pmp {

//=============================================================================

//! \addtogroup algorithms algorithms
//!@{

//! A class implementing a normal cone.
class NormalCone
{
public:
    //! default constructor (not initialized)
    NormalCone() {}

    //! Initialize cone with center (unit vector) and angle (radius in radians)
    NormalCone(const Normal& normal, Scalar angle = 0.0)
        : center_normal_(normal), angle_(angle)
    {
    }

    //! returns center normal
    const Normal& center_normal() const { return center_normal_; }

    //! returns size of cone (radius in radians)
    Scalar angle() const { return angle_; }

    //! merge *this with n.
    NormalCone& merge(const Normal& n) { return merge(NormalCone(n)); }

    //! merge *this with nc. *this will then enclose both cones.
    NormalCone& merge(const NormalCone& nc)
    {
        const Scalar dp = dot(center_normal_, nc.center_normal_);

        // axes point in same direction
        if (dp > 0.99999)
        {
            angle_ = std::max(angle_, nc.angle_);
        }

        // axes point in opposite directions
        else if (dp < -0.99999)
        {
            angle_ = 2 * M_PI;
        }

        else
        {
            // new angle
            Scalar center_angle = acos(dp);
            Scalar min_angle = std::min(-angle_, center_angle - nc.angle_);
            Scalar max_angle = std::max(angle_, center_angle + nc.angle_);
            angle_ = 0.5 * (max_angle - min_angle);

            // axis by SLERP
            Scalar axis_angle = 0.5 * (min_angle + max_angle);
            center_normal_ = ((center_normal_ * sin(center_angle - axis_angle) +
                               nc.center_normal_ * sin(axis_angle)) /
                              sin(center_angle));
        }

        return *this;
    }

private:
    Normal center_normal_;
    Scalar angle_;
};

//=============================================================================
//!@}
//=============================================================================
} // namespace pmp
//=============================================================================
