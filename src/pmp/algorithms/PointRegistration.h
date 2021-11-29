// Copyright 2011-2021 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include <vector>
#include "pmp/Types.h"

namespace pmp {

//! \addtogroup algorithms
//! @{

//! The two registration methods: Rigid registration optimizes for 
//! rotation and translation, similarity registration additionally for scaling.
enum Registration_method
{
    RIGID_REGISTRATION,
    SIMILARITY_REGISTRATION
};

//! Compute the rigid or similarity transform that best maps
//! all points in _src to _dst by minimizing the sum of squared
//! distances of point pairs (L2 metric). Through _weights one
//! can add a per-point weighting.
//! \details See \cite horn_2004_deftrans and \cite botsch_2006_deftrans for details.
mat4 registration(const std::vector<Point>& _src,
                  const std::vector<Point>& _dst,
                  Registration_method _mapping = RIGID_REGISTRATION,
                  const std::vector<Scalar>* _weights = nullptr);

//! Compute the rigid or similarity transform that best maps
//! all points in _src to _dst by minimizing the sum of
//! distances of point pairs (L1 metric).
//! \details See \cite bouaziz_2013_sparse for details. This implementation uses iteratively reweighted least squares.
mat4 registration_l1(const std::vector<Point>& _src,
                     const std::vector<Point>& _dst,
                     Registration_method _mapping = RIGID_REGISTRATION);

//! @}

} // namespace pmp
