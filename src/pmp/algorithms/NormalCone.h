//=============================================================================
// Copyright (C) 2011-2019 The pmp-library developers
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//=============================================================================
#pragma once
//=============================================================================

#include <pmp/types.h>

#include <cmath>

//=============================================================================

namespace pmp {

//=============================================================================

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
} // namespace pmp
//=============================================================================
