//=============================================================================
// Copyright (C) 2011-2017 The pmp-library developers
// All rights reserved.
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

#include <pmp/PointSet.h>

//=============================================================================

namespace pmp {

//=============================================================================

class PointBSPTree;

//=============================================================================

//! \addtogroup algorithms
//! @{

//=============================================================================

//! \brief A class for point set smoothing.
//! \details Smoothing is performed based on Moving Least Squares projection.
//! \pre Requires a PointSet with vertex normals
class PointSetSmoothing
{
public:
    //! Constructor
    PointSetSmoothing(PointSet& pointSet);

    //! \brief Smooth all points through MLS projection.
    void smooth();

private:
    //! Project a point \c x with normal \c n to the MLS surface
    void project(Point& x, Point& n, const PointBSPTree& tree) const;

    //! \brief Weight function.
    //! \details We use Wendland's compactly supported C2 function
    inline Scalar theta(Scalar x) const
    {
        if (x < m_radius)
        {
            x /= m_radius;
            return pow((1.0 - x), 4.0) * (4.0 * x + 1.0);
        }
        else
        {
            return 0;
        }
    }

    //! Weighted average for point position
    Point weightedAveragePosition(const Point& x, std::vector<int>& ball) const;

    //! weighted average for point normal
    Point weightedAverageNormal(const Point& x, std::vector<int>& ball) const;

private:
    PointSet&    m_pointSet; //!< the point set
    Scalar       m_radius;   //!< radius for ball queries
    Scalar       m_epsilon;  //!< projection error threshold
    unsigned int m_maxIter;  //!< maximal number of iterations threshold
};

//=============================================================================
//! @}
//=============================================================================
} // namespace pmp
//=============================================================================
