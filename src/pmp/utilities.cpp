// Copyright 2022 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/utilities.h"

namespace pmp {

BoundingBox bounds(const SurfaceMesh& mesh)
{
    BoundingBox bb;
    for (auto v : mesh.vertices())
        bb += mesh.position(v);
    return bb;
}

} // namespace pmp