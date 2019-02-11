//=============================================================================
// Copyright (C) 2013-2019 The pmp-library developers
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

//=============================================================================

namespace pmp {

//=============================================================================

//! Simple class for representing a bounding box
class BoundingBox
{

public:
    //! construct infinite/invalid bounding box
    BoundingBox()
        : min_(std::numeric_limits<Scalar>::max()),
          max_(-std::numeric_limits<Scalar>::max())
    {
    }

    //! construct from min and max points
    BoundingBox(const Point& min, const Point& max) : min_(min), max_(max) {}

    //! add point to bbox
    BoundingBox& operator+=(const Point& p)
    {
        for (int i = 0; i < 3; ++i)
        {
            if (p[i] < min_[i])
                min_[i] = p[i];
            else if (p[i] > max_[i])
                max_[i] = p[i];
        }
        return *this;
    }

    //! add two bboxes
    BoundingBox& operator+=(const BoundingBox& bb)
    {
        for (int i = 0; i < 3; ++i)
        {
            if (bb.min_[i] < min_[i])
                min_[i] = bb.min_[i];
            if (bb.max_[i] > max_[i])
                max_[i] = bb.max_[i];
        }
        return *this;
    }

    //! get min point
    Point& min() { return min_; }

    //! get max point
    Point& max() { return max_; }

    //! get center point
    Point center() const { return 0.5f * (min_ + max_); }

    //! indicate if bbox is empty
    bool is_empty() const
    {
        return (max_[0] < min_[0] || max_[1] < min_[1] ||
                max_[2] < min_[2]);
    }

    //! get size of the bbox
    Scalar size() const { return is_empty() ? 0.0 : distance(max_, min_); }

private:
    Point min_, max_;
};

//=============================================================================
} // namespace pmp
//=============================================================================
