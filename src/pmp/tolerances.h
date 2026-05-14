// Copyright 2026 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#pragma once

#include <type_traits>

namespace pmp::detail {

template <typename Scalar>
constexpr Scalar relative_epsilon()
{
    static_assert(std::is_floating_point_v<Scalar>);

    if constexpr (std::is_same_v<Scalar, float>)
        return Scalar(1e-6);
    else
        return Scalar(1e-12);
}

template <typename Scalar>
constexpr Scalar absolute_epsilon()
{
    static_assert(std::is_floating_point_v<Scalar>);

    if constexpr (std::is_same_v<Scalar, float>)
        return Scalar(1e-8);
    else
        return Scalar(1e-15);
}

} // namespace pmp::detail
