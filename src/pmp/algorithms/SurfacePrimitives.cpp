//=============================================================================
// Copyright (C) 2011-2020 The pmp-library developers
//
// This file is part of the Polygon Mesh Processing Library.
// Distributed under a MIT-style license, see LICENSE.txt for details.
//
// SPDX-License-Identifier: MIT-with-employer-disclaimer
//=============================================================================

#include "SurfacePrimitives.h"

namespace pmp {

void SurfacePrimitives::uv_sphere(const vec3& center, Scalar radius,
                                  size_t n_slices, size_t n_stacks)
{
    mesh_.clear();

    // add top vertex
    auto v0 = mesh_.add_vertex(Point(center[0], center[1] + radius, center[2]));

    // generate vertices per stack / slice
    for (size_t i = 0; i < n_stacks - 1; i++)
    {
        auto phi = M_PI * double(i + 1) / double(n_stacks);
        for (size_t j = 0; j < n_slices; ++j)
        {
            auto theta = 2.0 * M_PI * double(j) / double(n_slices);
            auto x = center[0] + radius * std::sin(phi) * std::cos(theta);
            auto y = center[1] + radius * std::cos(phi);
            auto z = center[2] + radius * std::sin(phi) * std::sin(theta);
            mesh_.add_vertex(Point(x, y, z));
        }
    }

    // add bottom vertex
    auto v1 = mesh_.add_vertex(Point(center[0], center[1] - radius, center[2]));

    // add top / bottom triangles
    size_t i0, i1;
    for (size_t i = 0; i < n_slices; ++i)
    {
        i0 = i + 1;
        i1 = (i + 1) % n_slices + 1;
        mesh_.add_triangle(v0, Vertex(i1), Vertex(i0));

        i0 = i + n_slices * (n_stacks - 2) + 1;
        i1 = (i + 1) % n_slices + n_slices * (n_stacks - 2) + 1;
        mesh_.add_triangle(v1, Vertex(i0), Vertex(i1));
    }

    // add quads per stack / slice
    size_t i2, i3;
    for (size_t j = 0; j < n_stacks - 2; ++j)
    {
        size_t idx0 = j * n_slices + 1;
        size_t idx1 = (j + 1) * n_slices + 1;
        for (size_t i = 0; i < n_slices; ++i)
        {
            i0 = idx0 + i;
            i1 = idx0 + (i + 1) % n_slices;
            i2 = idx1 + (i + 1) % n_slices;
            i3 = idx1 + i;
            mesh_.add_quad(Vertex(i0), Vertex(i1), Vertex(i2), Vertex(i3));
        }
    }
}

} // namespace pmp