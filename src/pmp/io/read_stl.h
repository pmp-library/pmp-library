// Copyright 2011-2022 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include <string>

#include "pmp/SurfaceMesh.h"

namespace pmp {

void read_stl(SurfaceMesh& mesh, const std::string& filename);

} // namespace pmp