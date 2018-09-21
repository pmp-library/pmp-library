//=============================================================================
// Copyright (C) 2013-2017 The pmp-library developers
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
        : m_min(std::numeric_limits<Scalar>::max()),
          m_max(-std::numeric_limits<Scalar>::max())
    {
    }

    //! construct from min and max points
    BoundingBox(const Point& min, const Point& max) : m_min(min), m_max(max) {}

    //! add point to bbox
    BoundingBox& operator+=(const Point& p)
    {
        for (int i = 0; i < 3; ++i)
        {
            if (p[i] < m_min[i])
                m_min[i] = p[i];
            else if (p[i] > m_max[i])
                m_max[i] = p[i];
        }
        return *this;
    }

    //! add two bboxes
    BoundingBox& operator+=(const BoundingBox& bb)
    {
        for (int i = 0; i < 3; ++i)
        {
            if (bb.m_min[i] < m_min[i])
                m_min[i] = bb.m_min[i];
            if (bb.m_max[i] > m_max[i])
                m_max[i] = bb.m_max[i];
        }
        return *this;
    }

    //! get min point
    Point& min() { return m_min; }

    //! get max point
    Point& max() { return m_max; }

    //! get center point
    Point center() const { return 0.5f * (m_min + m_max); }

    //! indicate if bbox is empty
    bool isEmpty() const
    {
        return (m_max[0] < m_min[0] || m_max[1] < m_min[1] ||
                m_max[2] < m_min[2]);
    }

    //! get size of the bbox
    Scalar size() const { return isEmpty() ? 0.0 : distance(m_max, m_min); }

private:
    Point m_min, m_max;
};

//=============================================================================
} // namespace pmp
//=============================================================================
